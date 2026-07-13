#include "fqtools/io/fastq_batch_pool.h"

#include <any>
#include <atomic>
#include <memory>
#include <mutex>
#include <utility>

#include "processing/execution_backend.h"
#include <tbb/global_control.h>
#include <tbb/parallel_pipeline.h>

namespace fq::processing {

namespace {

class OneTbbExecutionBackend final : public ExecutionBackend {
public:
    auto execute(ExecutionBackendContext context,
                 ExecutionOperation& operation) -> ErasedExecutionOutcome override {
        using BatchResult = std::pair<std::shared_ptr<fq::io::FastqBatch>, std::any>;

        auto batchPool = fq::io::createFastqBatchPool(context.config.maxLiveTokens,
                                                      context.config.maxLiveTokens * 2);
        auto result = operation.makeResult();
        std::atomic<std::uint64_t> batchCount{0};
        std::atomic<std::uint64_t> committedBytes{0};

        // serial_in_order 保证互斥；显式锁同时为 TSan 建立可见的 happens-before。
        std::mutex readerMutex;
        std::mutex writerMutex;

        tbb::global_control globalLimit(tbb::global_control::max_allowed_parallelism,
                                        context.config.threadCount);

        tbb::parallel_pipeline(
            context.config.maxLiveTokens,
            tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
                tbb::filter_mode::serial_in_order,
                [&context, batchPool, &readerMutex](
                    tbb::flow_control& flow) -> std::shared_ptr<fq::io::FastqBatch> {
                    auto batch = batchPool->acquire();
                    {
                        std::lock_guard lock(readerMutex);
                        if (context.reader->nextBatch(*batch, context.batchSize)) {
                            return batch;
                        }
                    }
                    flow.stop();
                    return nullptr;
                }) &
                tbb::make_filter<std::shared_ptr<fq::io::FastqBatch>, BatchResult>(
                    tbb::filter_mode::parallel,
                    [&operation](std::shared_ptr<fq::io::FastqBatch> batch) -> BatchResult {
                        auto partial = operation.processBatch(*batch);
                        return {std::move(batch), std::move(partial)};
                    }) &
                tbb::make_filter<BatchResult, void>(
                    tbb::filter_mode::serial_in_order,
                    [&context, &operation, &result, &batchCount, &committedBytes, &writerMutex](
                        BatchResult batchResult) {
                        std::lock_guard lock(writerMutex);
                        const auto bytes =
                            context.writer ? context.writer->write(*batchResult.first) : 0;
                        operation.afterCommit(batchResult.second, bytes);
                        operation.merge(result, std::move(batchResult.second));
                        batchCount.fetch_add(1, std::memory_order_relaxed);
                        committedBytes.fetch_add(bytes, std::memory_order_relaxed);
                    }));

        // pipeline 完成后显式建立 reader/writer 与调用线程之间的内存序。
        {
            std::lock_guard readerLock(readerMutex);
            std::lock_guard writerLock(writerMutex);
        }

        return {std::move(result),
                {batchCount.load(std::memory_order_relaxed),
                 committedBytes.load(std::memory_order_relaxed)}};
    }
};

}  // namespace

auto createOneTbbExecutionBackend() -> std::unique_ptr<ExecutionBackend> {
    return std::make_unique<OneTbbExecutionBackend>();
}

}  // namespace fq::processing
