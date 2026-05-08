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

class FileBackedAdapter final : public fq::processing::ExecutionRuntimeAdapter {
public:
    FileBackedAdapter(const fq::processing::ExecutionRuntimePlan& plan,
                      const fq::processing::RuntimePolicy& runtimePolicy)
        : reader_(plan.inputPath, makeReaderOptions(runtimePolicy)),
          writer_(plan.outputPath ? std::make_optional<fq::io::FastqWriter>(
                                        *plan.outputPath, makeWriterOptions(runtimePolicy))
                                  : std::nullopt) {}

    auto nextBatch(fq::io::FastqBatch& batch, size_t maxRecords) -> bool override {
        return reader_.nextBatch(batch, maxRecords);
    }

    auto commit(const fq::io::FastqBatch& batch) -> std::uint64_t override {
        if (!writer_) {
            return 0;
        }
        const auto before = writer_->totalUncompressedBytes();
        writer_->write(batch);
        return writer_->totalUncompressedBytes() - before;
    }

private:
    fq::io::FastqReader reader_;
    std::optional<fq::io::FastqWriter> writer_;
};

class IOBackedAdapter final : public fq::processing::ExecutionRuntimeAdapter {
public:
    IOBackedAdapter(std::unique_ptr<fq::io::IReader> reader,
                    std::shared_ptr<fq::io::IWriter> writer)
        : reader_(std::move(reader)), writer_(std::move(writer)) {}

    auto nextBatch(fq::io::FastqBatch& batch, size_t maxRecords) -> bool override {
        // Try to use maxRecords if the reader is a FastqReader
        // This handles the case where partial custom I/O uses file-backed reader
        if (auto* fastqReader = dynamic_cast<fq::io::FastqReader*>(reader_.get())) {
            return fastqReader->nextBatch(batch, maxRecords);
        }
        // Fall back to default nextBatch for generic IReader
        return reader_->nextBatch(batch);
    }

    auto commit(const fq::io::FastqBatch& batch) -> std::uint64_t override {
        if (!writer_) {
            return 0;
        }
        if (auto* fastqWriter = dynamic_cast<fq::io::FastqWriter*>(writer_.get())) {
            const auto before = fastqWriter->totalUncompressedBytes();
            writer_->write(batch);
            return fastqWriter->totalUncompressedBytes() - before;
        }
        writer_->write(batch);
        return batch.buffer().size();
    }

private:
    std::unique_ptr<fq::io::IReader> reader_;
    std::shared_ptr<fq::io::IWriter> writer_;
};

}  // namespace

namespace fq::processing {

ExecutionRuntime::ExecutionRuntime() = default;

ExecutionRuntime::ExecutionRuntime(std::unique_ptr<ExecutionRuntimeAdapter> adapter)
    : adapter_(std::move(adapter)), hasFixedAdapter_(true) {
    if (!adapter_) {
        throw std::invalid_argument("ExecutionRuntime: adapter cannot be null");
    }
}

void ExecutionRuntime::setCustomReader(std::unique_ptr<fq::io::IReader> reader) {
    customReaderConfigured_ = static_cast<bool>(reader);
    customReader_ = std::move(reader);
    if (!hasFixedAdapter_) {
        adapter_.reset();
    }
}

void ExecutionRuntime::setCustomWriter(std::shared_ptr<fq::io::IWriter> writer) {
    customWriter_ = std::move(writer);
    if (!hasFixedAdapter_) {
        adapter_.reset();
    }
}

auto ExecutionRuntime::derivePolicy(const ExecutionRuntimePlan& plan) const -> RuntimePolicy {
    return deriveRuntimePolicy(plan.options);
}

auto ExecutionRuntime::deriveExecutionPlan(const ExecutionRuntimePlan& plan,
                                           const RuntimePolicy& runtimePolicy) const
    -> PipelineExecutionPlan {
    return derivePipelineExecutionPlan(
        plan.options, runtimePolicy, customReader_ != nullptr, customWriter_ != nullptr);
}

auto ExecutionRuntime::createFileReader(const ExecutionRuntimePlan& plan,
                                        const RuntimePolicy& runtimePolicy) const
    -> std::shared_ptr<fq::io::FastqReader> {
    auto reader =
        std::make_shared<fq::io::FastqReader>(plan.inputPath, makeReaderOptions(runtimePolicy));
    if (!reader->isOpen()) {
        throw std::runtime_error("Failed to open input file: " + plan.inputPath);
    }
    return reader;
}

auto ExecutionRuntime::createFileWriter(const ExecutionRuntimePlan& plan,
                                        const RuntimePolicy& runtimePolicy) const
    -> std::optional<fq::io::FastqWriter> {
    if (!plan.outputPath) {
        return std::nullopt;
    }

    auto writer = fq::io::FastqWriter(*plan.outputPath, makeWriterOptions(runtimePolicy));
    if (!writer.isOpen()) {
        throw std::runtime_error("Failed to open output file: " + *plan.outputPath);
    }
    return writer;
}

auto ExecutionRuntime::ensureAdapter(const ExecutionRuntimePlan& plan,
                                     const RuntimePolicy& runtimePolicy)
    -> ExecutionRuntimeAdapter& {
    if (hasFixedAdapter_) {
        return *adapter_;
    }

    const bool hasCustomReader = customReader_ != nullptr;
    const bool hasCustomWriter = customWriter_ != nullptr;

    if (hasCustomReader && hasCustomWriter) {
        adapter_ = std::make_unique<IOBackedAdapter>(std::move(customReader_), customWriter_);
    } else if (hasCustomReader && !hasCustomWriter) {
        std::shared_ptr<fq::io::IWriter> writer;
        if (plan.outputPath) {
            writer = std::make_shared<fq::io::FastqWriter>(*plan.outputPath,
                                                           makeWriterOptions(runtimePolicy));
        }
        adapter_ = std::make_unique<IOBackedAdapter>(std::move(customReader_), writer);
    } else if (!hasCustomReader && hasCustomWriter) {
        auto reader =
            std::make_unique<fq::io::FastqReader>(plan.inputPath, makeReaderOptions(runtimePolicy));
        adapter_ = std::make_unique<IOBackedAdapter>(std::move(reader), customWriter_);
    } else {
        adapter_ = std::make_unique<FileBackedAdapter>(plan, runtimePolicy);
    }
    return *adapter_;
}

auto makeAdapterForTesting(std::unique_ptr<fq::io::IReader> reader,
                           std::shared_ptr<fq::io::IWriter> writer)
    -> std::unique_ptr<ExecutionRuntimeAdapter> {
    return std::make_unique<IOBackedAdapter>(std::move(reader), std::move(writer));
}

}  // namespace fq::processing
