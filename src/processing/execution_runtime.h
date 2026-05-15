#pragma once

#include "fqtools/io/fastq_batch_pool.h"
#include "fqtools/io/fastq_io.h"
#include "fqtools/io/fastq_reader.h"
#include "fqtools/io/fastq_writer.h"
#include "fqtools/io/reader_interface.h"
#include "fqtools/io/writer_interface.h"
#include "fqtools/processing/processing_options.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

#include "processing/pipeline_execution_plan.h"
#include "processing/runtime_policy.h"
#include <tbb/global_control.h>
#include <tbb/parallel_pipeline.h>

namespace fq::processing {

struct ExecutionRuntimeRequest {
    std::string inputPath;
    std::optional<std::string> outputPath;
    ProcessingOptions options;
};

using ExecutionRuntimePlan = ExecutionRuntimeRequest;

struct ExecutionRuntimeMetrics {
    std::uint64_t batchCount = 0;
    std::uint64_t committedBytes = 0;
};

template <typename Result>
struct ExecutionRuntimeOutcome {
    Result result;
    ExecutionRuntimeMetrics metrics;
};

class ExecutionRuntime {
public:
    ExecutionRuntime() = default;
    ExecutionRuntime(std::unique_ptr<fq::io::IReader> customReader,
                     std::shared_ptr<fq::io::IWriter> customWriter = {});

    template <typename Adapter>
    auto execute(const ExecutionRuntimeRequest& request, Adapter&& adapter)
        -> ExecutionRuntimeOutcome<typename std::decay_t<Adapter>::result_type> {
        using Result = typename std::decay_t<Adapter>::result_type;

        ExecutionRuntimeMetrics metrics;
        auto result = run<Result>(
            request,
            [&adapter](fq::io::FastqBatch& batch) { return adapter.processBatch(batch); },
            [&adapter](Result& total, Result partial) { adapter.merge(total, std::move(partial)); },
            [&adapter, &metrics](Result& partial, std::uint64_t committedBytes) {
                ++metrics.batchCount;
                metrics.committedBytes += committedBytes;
                adapter.afterCommit(partial, committedBytes);
            },
            adapter.makeResult());
        return {std::move(result), metrics};
    }

private:
    template <typename Final, typename BatchWork, typename Reduce, typename AfterCommit>
    auto run(const ExecutionRuntimePlan& plan,
             BatchWork&& batchWork,
             Reduce&& reduce,
             AfterCommit&& afterCommit,
             Final initial) -> Final {
        plan.options.validate();
        if (customReaderConfigured_ && !customReader_) {
            throw std::invalid_argument(
                "ExecutionRuntime: custom reader must be reset before rerunning");
        }

        const auto runtimePolicy = derivePolicy(plan);
        const auto executionPlan = deriveExecutionPlan(plan, runtimePolicy);
        if (executionPlan.mode == PipelineExecutionMode::Parallel) {
            return runParallel(plan,
                               runtimePolicy,
                               executionPlan,
                               std::forward<BatchWork>(batchWork),
                               std::forward<Reduce>(reduce),
                               std::forward<AfterCommit>(afterCommit),
                               std::move(initial));
        }

        return runSequential(plan,
                             runtimePolicy,
                             std::forward<BatchWork>(batchWork),
                             std::forward<Reduce>(reduce),
                             std::forward<AfterCommit>(afterCommit),
                             std::move(initial));
    }

    template <typename Final, typename BatchWork, typename Reduce, typename AfterCommit>
    auto runSequential(const ExecutionRuntimePlan& plan,
                       const RuntimePolicy& runtimePolicy,
                       BatchWork&& batchWork,
                       Reduce&& reduce,
                       AfterCommit&& afterCommit,
                       Final initial) -> Final {
        auto reader = createReader(plan, runtimePolicy);
        auto writer = createWriter(plan, runtimePolicy);
        fq::io::FastqBatch batch;
        Final finalResult = std::move(initial);
        while (nextBatch(*reader, batch, plan.options.batchSize)) {
            auto partial = batchWork(batch);
            const auto committedBytes = commitBatch(writer, batch);
            afterCommit(partial, committedBytes);
            reduce(finalResult, std::move(partial));
        }
        return finalResult;
    }

    template <typename Final, typename BatchWork, typename Reduce, typename AfterCommit>
    auto runParallel(const ExecutionRuntimePlan& plan,
                     const RuntimePolicy& runtimePolicy,
                     const PipelineExecutionPlan& executionPlan,
                     BatchWork&& batchWork,
                     Reduce&& reduce,
                     AfterCommit&& afterCommit,
                     Final initial) -> Final {
        using Partial = std::decay_t<std::invoke_result_t<BatchWork&, fq::io::FastqBatch&>>;
        using BatchResult = std::pair<std::shared_ptr<fq::io::FastqBatch>, Partial>;

        auto reader = std::shared_ptr<fq::io::IReader>(createReader(plan, runtimePolicy).release());
        auto writer = createWriter(plan, runtimePolicy);
        auto batchPool = fq::io::createFastqBatchPool(executionPlan.maxLiveTokens,
                                                      executionPlan.maxLiveTokens * 2);
        Final finalResult = std::move(initial);

        tbb::global_control globalLimit(tbb::global_control::max_allowed_parallelism,
                                        executionPlan.threadCount);

        tbb::parallel_pipeline(
            executionPlan.maxLiveTokens,
            tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
                tbb::filter_mode::serial_in_order,
                [this, reader = std::move(reader), batchPool, batchSize = plan.options.batchSize](
                    tbb::flow_control& fc) -> std::shared_ptr<fq::io::FastqBatch> {
                    auto batch = batchPool->acquire();
                    if (nextBatch(*reader, *batch, batchSize)) {
                        return batch;
                    }
                    fc.stop();
                    return nullptr;
                }) &
                tbb::make_filter<std::shared_ptr<fq::io::FastqBatch>, BatchResult>(
                    tbb::filter_mode::parallel,
                    [work = std::forward<BatchWork>(batchWork)](
                        std::shared_ptr<fq::io::FastqBatch> batch) -> BatchResult {
                        auto partial = work(*batch);
                        return std::make_pair(std::move(batch), std::move(partial));
                    }) &
                tbb::make_filter<BatchResult, void>(
                    tbb::filter_mode::serial_in_order,
                    [this, writer = std::move(writer), &reduce, &afterCommit, &finalResult](
                        BatchResult result) {
                        const auto committedBytes = commitBatch(writer, *result.first);
                        afterCommit(result.second, committedBytes);
                        reduce(finalResult, std::move(result.second));
                    }));

        return finalResult;
    }

    auto derivePolicy(const ExecutionRuntimePlan& plan) const -> RuntimePolicy;
    auto deriveExecutionPlan(const ExecutionRuntimePlan& plan,
                             const RuntimePolicy& runtimePolicy) const -> PipelineExecutionPlan;
    auto createReader(const ExecutionRuntimePlan& plan,
                      const RuntimePolicy& runtimePolicy) -> std::unique_ptr<fq::io::IReader>;
    auto createWriter(const ExecutionRuntimePlan& plan,
                      const RuntimePolicy& runtimePolicy) const -> std::shared_ptr<fq::io::IWriter>;
    auto nextBatch(fq::io::IReader& reader,
                   fq::io::FastqBatch& batch,
                   size_t maxRecords) const -> bool;
    auto commitBatch(const std::shared_ptr<fq::io::IWriter>& writer,
                     const fq::io::FastqBatch& batch) const -> std::uint64_t;

    std::unique_ptr<fq::io::IReader> customReader_;
    std::shared_ptr<fq::io::IWriter> customWriter_;
    bool customReaderConfigured_ = false;
};

}  // namespace fq::processing
