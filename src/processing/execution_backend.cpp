#include "processing/execution_backend.h"

#include "fqtools/io/fastq_io.h"

#include <utility>

namespace fq::processing {

namespace {

class SequentialExecutionBackend final : public ExecutionBackend {
public:
    auto execute(ExecutionBackendContext context,
                 ExecutionOperation& operation) -> ErasedExecutionOutcome override {
        auto result = operation.makeResult();
        ExecutionRuntimeMetrics metrics;
        fq::io::FastqBatch batch(context.config.batchCapacityBytes);

        while (context.reader->nextBatch(batch, context.batchSize)) {
            auto partial = operation.processBatch(batch);
            const auto committedBytes = context.writer ? context.writer->write(batch) : 0;
            operation.afterCommit(partial, committedBytes);
            operation.merge(result, std::move(partial));
            ++metrics.batchCount;
            metrics.committedBytes += committedBytes;
        }

        return {std::move(result), metrics};
    }
};

}  // namespace

auto createSequentialExecutionBackend() -> std::unique_ptr<ExecutionBackend> {
    return std::make_unique<SequentialExecutionBackend>();
}

}  // namespace fq::processing
