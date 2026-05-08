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

struct ExecutionRuntimePlan {
    std::string inputPath;
    std::optional<std::string> outputPath;
    ProcessingOptions options;
};

class ExecutionRuntimeAdapter {
public:
    virtual ~ExecutionRuntimeAdapter() = default;
    virtual auto nextBatch(fq::io::FastqBatch& batch, size_t maxRecords) -> bool = 0;
    virtual auto commit(const fq::io::FastqBatch& batch) -> std::uint64_t = 0;
};

class ExecutionRuntime {
public:
    ExecutionRuntime();
    explicit ExecutionRuntime(std::unique_ptr<ExecutionRuntimeAdapter> adapter);

    void setCustomReader(std::unique_ptr<fq::io::IReader> reader);
    void setCustomWriter(std::shared_ptr<fq::io::IWriter> writer);

    template <typename Final, typename BatchWork, typename Reduce, typename AfterCommit>
    auto run(const ExecutionRuntimePlan& plan,
             BatchWork&& batchWork,
             Reduce&& reduce,
             AfterCommit&& afterCommit,
             Final initial) -> Final {
        plan.options.validate();
        if (!hasFixedAdapter_ && customReaderConfigured_ && !customReader_) {
            throw std::invalid_argument(
                "ExecutionRuntime: custom reader must be reset before rerunning");
        }

        const auto runtimePolicy = derivePolicy(plan);
        const auto executionPlan = deriveExecutionPlan(plan, runtimePolicy);
        if (!hasFixedAdapter_ && executionPlan.mode == PipelineExecutionMode::Parallel) {
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

private:
    template <typename Final, typename BatchWork, typename Reduce, typename AfterCommit>
    auto runSequential(const ExecutionRuntimePlan& plan,
                       const RuntimePolicy& runtimePolicy,
                       BatchWork&& batchWork,
                       Reduce&& reduce,
                       AfterCommit&& afterCommit,
                       Final initial) -> Final {
        struct AdapterResetGuard {
            ExecutionRuntime* runtime;

            ~AdapterResetGuard() {
                if (!runtime->hasFixedAdapter_) {
                    runtime->adapter_.reset();
                }
            }
        } resetGuard{this};

        auto& adapter = hasFixedAdapter_ ? *adapter_ : ensureAdapter(plan, runtimePolicy);
        fq::io::FastqBatch batch;
        Final finalResult = std::move(initial);
        while (adapter.nextBatch(batch, plan.options.batchSize)) {
            auto partial = batchWork(batch);
            const auto committedBytes = adapter.commit(batch);
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

        auto reader = createFileReader(plan, runtimePolicy);
        auto writer = createFileWriter(plan, runtimePolicy);
        auto batchPool = fq::io::createFastqBatchPool(executionPlan.maxLiveTokens,
                                                      executionPlan.maxLiveTokens * 2);
        Final finalResult = std::move(initial);

        tbb::global_control globalLimit(tbb::global_control::max_allowed_parallelism,
                                        executionPlan.threadCount);

        tbb::parallel_pipeline(
            executionPlan.maxLiveTokens,
            tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
                tbb::filter_mode::serial_in_order,
                [reader, batchPool, batchSize = plan.options.batchSize](
                    tbb::flow_control& fc) -> std::shared_ptr<fq::io::FastqBatch> {
                    auto batch = batchPool->acquire();
                    if (reader->nextBatch(*batch, batchSize)) {
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
                    [&writer, &reduce, &afterCommit, &finalResult](BatchResult result) {
                        std::uint64_t committedBytes = 0;
                        if (writer) {
                            const auto before = writer->totalUncompressedBytes();
                            writer->write(*result.first);
                            committedBytes = writer->totalUncompressedBytes() - before;
                        }
                        afterCommit(result.second, committedBytes);
                        reduce(finalResult, std::move(result.second));
                    }));

        return finalResult;
    }

    auto derivePolicy(const ExecutionRuntimePlan& plan) const -> RuntimePolicy;
    auto deriveExecutionPlan(const ExecutionRuntimePlan& plan,
                             const RuntimePolicy& runtimePolicy) const -> PipelineExecutionPlan;
    auto createFileReader(const ExecutionRuntimePlan& plan, const RuntimePolicy& runtimePolicy)
        const -> std::shared_ptr<fq::io::FastqReader>;
    auto createFileWriter(const ExecutionRuntimePlan& plan, const RuntimePolicy& runtimePolicy)
        const -> std::optional<fq::io::FastqWriter>;
    auto ensureAdapter(const ExecutionRuntimePlan& plan,
                       const RuntimePolicy& runtimePolicy) -> ExecutionRuntimeAdapter&;

    std::unique_ptr<fq::io::IReader> customReader_;
    std::shared_ptr<fq::io::IWriter> customWriter_;
    std::unique_ptr<ExecutionRuntimeAdapter> adapter_;
    bool hasFixedAdapter_ = false;
    bool customReaderConfigured_ = false;
};

auto makeAdapterForTesting(std::unique_ptr<fq::io::IReader> reader,
                           std::shared_ptr<fq::io::IWriter> writer)
    -> std::unique_ptr<ExecutionRuntimeAdapter>;

}  // namespace fq::processing
