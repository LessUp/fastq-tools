#include "processing/execution_backend.h"

#if defined(FQ_ENABLE_TASKFLOW_BACKEND)

#include <any>
#include <atomic>
#include <bit>
#include <memory>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

#include "processing/fastq_batch_pool.h"
#include <taskflow/algorithm/pipeline.hpp>
#include <taskflow/taskflow.hpp>

namespace fq::processing {

namespace {

struct TaskflowLineState {
    std::shared_ptr<fq::io::FastqBatch> batch;
    std::any partial;
};

class TaskflowExecutionBackend final : public ExecutionBackend {
public:
    auto execute(ExecutionBackendContext context, ExecutionOperation& operation)
        -> ErasedExecutionOutcome override {
        auto batchPool = createFastqBatchPool(context.config.maxLiveTokens,
                                              context.config.maxLiveTokens * 2,
                                              context.config.batchCapacityBytes,
                                              context.batchSize);
        std::vector<std::optional<TaskflowLineState>> lines(context.config.maxLiveTokens);
        auto result = operation.makeResult();
        std::atomic<std::uint64_t> batchCount{0};
        std::atomic<std::uint64_t> committedBytes{0};
        std::mutex readerMutex;
        std::mutex writerMutex;

        tf::Pipeline pipeline(
            context.config.maxLiveTokens,
            tf::Pipe{tf::PipeType::SERIAL,
                     [&context, batchPool, &lines, &readerMutex](tf::Pipeflow& flow) {
                         auto batch = batchPool->acquire();
                         {
                             std::lock_guard lock(readerMutex);
                             if (!context.reader->nextBatch(*batch, context.batchSize)) {
                                 flow.stop();
                                 return;
                             }
                         }
                         lines[flow.line()].emplace(
                             TaskflowLineState{std::move(batch), std::any{}});
                     }},
            tf::Pipe{tf::PipeType::PARALLEL,
                     [&operation, &lines](tf::Pipeflow& flow) {
                         auto& line = *lines[flow.line()];
                         line.partial = operation.processBatch(*line.batch);
                     }},
            tf::Pipe{
                tf::PipeType::SERIAL,
                [&context, &operation, &lines, &result, &batchCount, &committedBytes, &writerMutex](
                    tf::Pipeflow& flow) {
                    auto& line = *lines[flow.line()];
                    std::lock_guard lock(writerMutex);
                    const auto bytes = context.writer ? context.writer->write(*line.batch) : 0;
                    operation.afterCommit(line.partial, bytes);
                    operation.merge(result, std::move(line.partial));
                    lines[flow.line()].reset();
                    batchCount.fetch_add(1, std::memory_order_relaxed);
                    committedBytes.fetch_add(bytes, std::memory_order_relaxed);
                }});

        tf::Taskflow taskflow("fastq-runtime");
        taskflow.composed_of(pipeline).name("fastq-pipeline");
        tf::Executor executor(context.config.threadCount);
        executor.run(taskflow).get();

        if (context.writer) {
            context.writer->finish();
        }

        return {std::move(result),
                {batchCount.load(std::memory_order_relaxed),
                 committedBytes.load(std::memory_order_relaxed)}};
    }
};

}  // namespace

auto createTaskflowExecutionBackend() -> std::unique_ptr<ExecutionBackend> {
    return std::make_unique<TaskflowExecutionBackend>();
}

auto isTaskflowExecutionBackendAvailable() noexcept -> bool {
    return true;
}

}  // namespace fq::processing

#else

namespace fq::processing {

auto createTaskflowExecutionBackend() -> std::unique_ptr<ExecutionBackend> {
    return {};
}

auto isTaskflowExecutionBackendAvailable() noexcept -> bool {
    return false;
}

}  // namespace fq::processing

#endif
