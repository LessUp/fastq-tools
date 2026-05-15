#include "processing/execution_runtime.h"

#include <stdexcept>

namespace {

auto makeReaderOptions(const fq::processing::RuntimePolicy& runtimePolicy)
    -> fq::io::FastqReaderOptions {
    fq::io::FastqReaderOptions options;
    options.readChunkBytes = runtimePolicy.readChunkBytes;
    options.zlibBufferBytes = runtimePolicy.zlibBufferBytes;
    options.maxBufferBytes = runtimePolicy.batchCapacityBytes;
    return options;
}

auto makeWriterOptions(const fq::processing::RuntimePolicy& runtimePolicy)
    -> fq::io::FastqWriterOptions {
    fq::io::FastqWriterOptions options;
    options.zlibBufferBytes = runtimePolicy.zlibBufferBytes;
    options.outputBufferBytes = runtimePolicy.writerBufferBytes;
    return options;
}

}  // namespace

namespace fq::processing {

ExecutionRuntime::ExecutionRuntime(std::unique_ptr<fq::io::IReader> customReader,
                                   std::shared_ptr<fq::io::IWriter> customWriter)
    : customReader_(std::move(customReader)),
      customWriter_(std::move(customWriter)),
      customReaderConfigured_(customReader_ != nullptr) {}

auto ExecutionRuntime::derivePolicy(const ExecutionRuntimePlan& plan) const -> RuntimePolicy {
    return deriveRuntimePolicy(plan.options);
}

auto ExecutionRuntime::deriveExecutionPlan(const ExecutionRuntimePlan& plan,
                                           const RuntimePolicy& runtimePolicy) const
    -> PipelineExecutionPlan {
    return derivePipelineExecutionPlan(
        plan.options, runtimePolicy, customReader_ != nullptr, customWriter_ != nullptr);
}

auto ExecutionRuntime::createReader(const ExecutionRuntimePlan& plan,
                                    const RuntimePolicy& runtimePolicy)
    -> std::unique_ptr<fq::io::IReader> {
    if (customReader_) {
        return std::move(customReader_);
    }

    auto reader =
        std::make_unique<fq::io::FastqReader>(plan.inputPath, makeReaderOptions(runtimePolicy));
    if (!reader->isOpen()) {
        throw std::runtime_error("Failed to open input file: " + plan.inputPath);
    }
    return reader;
}

auto ExecutionRuntime::createWriter(const ExecutionRuntimePlan& plan,
                                    const RuntimePolicy& runtimePolicy) const
    -> std::shared_ptr<fq::io::IWriter> {
    if (customWriter_) {
        return customWriter_;
    }
    if (!plan.outputPath) {
        return {};
    }

    auto writer =
        std::make_shared<fq::io::FastqWriter>(*plan.outputPath, makeWriterOptions(runtimePolicy));
    if (!writer->isOpen()) {
        throw std::runtime_error("Failed to open output file: " + *plan.outputPath);
    }
    return writer;
}

auto ExecutionRuntime::nextBatch(fq::io::IReader& reader,
                                 fq::io::FastqBatch& batch,
                                 size_t maxRecords) const -> bool {
    if (auto* fastqReader = dynamic_cast<fq::io::FastqReader*>(&reader)) {
        return fastqReader->nextBatch(batch, maxRecords);
    }
    return reader.nextBatch(batch);
}

auto ExecutionRuntime::commitBatch(const std::shared_ptr<fq::io::IWriter>& writer,
                                   const fq::io::FastqBatch& batch) const -> std::uint64_t {
    if (!writer) {
        return 0;
    }

    if (auto* fastqWriter = dynamic_cast<fq::io::FastqWriter*>(writer.get())) {
        const auto before = fastqWriter->totalUncompressedBytes();
        writer->write(batch);
        return fastqWriter->totalUncompressedBytes() - before;
    }

    writer->write(batch);
    return batch.buffer().size();
}

}  // namespace fq::processing
