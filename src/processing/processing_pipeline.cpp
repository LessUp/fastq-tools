#include "processing/processing_pipeline.h"

#include "fqtools/io/fastq_batch_pool.h"
#include "fqtools/io/fastq_reader.h"
#include "fqtools/io/fastq_writer.h"
#include "fqtools/logging.h"
#include "fqtools/processing/read_mutator_interface.h"
#include "fqtools/processing/read_predicate_interface.h"

#include <algorithm>
#include <stdexcept>

#include <tbb/global_control.h>
#include <tbb/parallel_pipeline.h>

namespace fq::processing {

namespace {

auto resolveMaxInFlightBatches(size_t configuredMaxInFlightBatches,
                               size_t memoryLimitBytes,
                               size_t batchCapacityBytes,
                               size_t threadCount) -> size_t {
    size_t maxTokens = std::max(static_cast<size_t>(4), threadCount * 2);
    if (configuredMaxInFlightBatches > 0) {
        maxTokens = configuredMaxInFlightBatches;
    }
    if (memoryLimitBytes > 0 && batchCapacityBytes > 0) {
        const size_t cap = (memoryLimitBytes * 7 / 10) / batchCapacityBytes;
        if (cap > 0) {
            maxTokens = std::min(maxTokens, cap);
        }
    }
    return std::max(static_cast<size_t>(1), maxTokens);
}

}  // namespace

ProcessingPipeline::ProcessingPipeline() = default;
ProcessingPipeline::~ProcessingPipeline() = default;

void ProcessingPipeline::setInputPath(const std::string& inputPath) {
    inputPath_ = inputPath;
}
void ProcessingPipeline::setOutputPath(const std::string& outputPath) {
    outputPath_ = outputPath;
}
void ProcessingPipeline::setProcessingConfig(const ProcessingConfig& config) {
    config_ = config;
}
void ProcessingPipeline::addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) {
    mutators_.push_back(std::move(mutator));
}
void ProcessingPipeline::addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) {
    predicates_.push_back(std::move(predicate));
}

auto ProcessingPipeline::run() -> ProcessingStatistics {
    switch (config_.executionBackend) {
    case ExecutionBackend::OneTbb:
        if (config_.threadCount > 1) {
            return processWithTBB();
        }
        return processSequential();
    }

    throw std::invalid_argument("Unsupported execution backend for processing pipeline.");
}

auto ProcessingPipeline::processSequential() -> ProcessingStatistics {
    ProcessingStatistics stats;
    stats.allocationTelemetryEnabled = config_.allocationTelemetryEnabled;
    stats.memoryResourcePolicy = config_.memoryResourcePolicy;
    stats.resolvedMaxInFlightBatches = 1;

    try {
        fq::io::FastqReaderOptions readerOptions;
        readerOptions.readChunkBytes = config_.readChunkBytes;
        readerOptions.zlibBufferBytes = config_.zlibBufferBytes;
        readerOptions.maxBufferBytes = config_.batchCapacityBytes;

        fq::io::FastqReader reader(inputPath_, readerOptions);
        if (!reader.isOpen()) {
            throw std::runtime_error("Failed to open input file: " + inputPath_);
        }

        fq::io::FastqWriterOptions writerOptions;
        writerOptions.zlibBufferBytes = config_.zlibBufferBytes;
        writerOptions.outputBufferBytes = config_.writerBufferBytes;

        fq::io::FastqWriter writer(outputPath_, writerOptions);
        if (!writer.isOpen()) {
            throw std::runtime_error("Failed to open output file: " + outputPath_);
        }

        fq::io::FastqBatch batch(config_.batchCapacityBytes, config_.batchSize);
        auto startTime = std::chrono::steady_clock::now();

        while (reader.nextBatch(batch, config_.batchSize)) {
            processBatch(batch, stats);
            writer.write(batch);
        }

        auto endTime = std::chrono::steady_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        stats.elapsedMs = static_cast<uint64_t>(duration);
        stats.processingTimeMs = static_cast<double>(stats.elapsedMs);
        stats.outputBytes = writer.totalUncompressedBytes();
        if (stats.elapsedMs > 0) {
            stats.throughputMbps = (static_cast<double>(stats.outputBytes) / 1024.0 / 1024.0) /
                (static_cast<double>(stats.elapsedMs) / 1000.0);
        }
    } catch (const std::exception& e) {
        fq::logging::error("Error in sequential processing: {}", e.what());
        throw;
    }

    return stats;
}

auto ProcessingPipeline::processBatch(fq::io::FastqBatch& batch,
                                      ProcessingStatistics& stats) -> bool {
    stats.inputBytes += batch.buffer().size();
    auto& records = batch.records();
    const size_t totalInBatch = records.size();
    size_t passedCount = 0;
    const bool hasPredicates = !predicates_.empty();
    const bool hasMutators = !mutators_.empty();

    size_t modifiedCount = 0;

    for (size_t i = 0; i < totalInBatch; ++i) {
        auto& read = records[i];

        bool passed = true;
        if (hasPredicates) {
            for (const auto& predicate : predicates_) {
                if (!predicate->evaluate(read)) {
                    passed = false;
                    break;
                }
            }
        }

        if (passed && hasMutators) {
            const size_t originalLen = read.length();
            for (const auto& mutator : mutators_) {
                mutator->process(read);
            }
            if (read.empty()) {
                passed = false;
            } else if (read.length() != originalLen) {
                ++modifiedCount;
            }
        }

        if (passed) {
            if (passedCount != i) {
                records[passedCount] = read;
            }
            passedCount++;
        }
    }

    // 批量更新统计，避免循环内逐条累加
    stats.totalReads += totalInBatch;
    stats.passedReads += passedCount;
    stats.filteredReads += (totalInBatch - passedCount);
    stats.modifiedReads += modifiedCount;
    records.resize(passedCount);

    return true;
}

auto ProcessingPipeline::processWithTBB() -> ProcessingStatistics {
    ProcessingStatistics finalStats;
    finalStats.allocationTelemetryEnabled = config_.allocationTelemetryEnabled;
    finalStats.memoryResourcePolicy = config_.memoryResourcePolicy;
    auto startTime = std::chrono::steady_clock::now();

    const size_t threadCount = std::max<size_t>(1, config_.threadCount);
    tbb::global_control globalLimit(tbb::global_control::max_allowed_parallelism, threadCount);

    fq::io::FastqReaderOptions readerOptions;
    readerOptions.readChunkBytes = config_.readChunkBytes;
    readerOptions.zlibBufferBytes = config_.zlibBufferBytes;
    readerOptions.maxBufferBytes = config_.batchCapacityBytes;

    auto reader = std::make_shared<fq::io::FastqReader>(inputPath_, readerOptions);
    if (!reader->isOpen())
        throw std::runtime_error("Failed to open input file: " + inputPath_);

    fq::io::FastqWriterOptions writerOptions;
    writerOptions.zlibBufferBytes = config_.zlibBufferBytes;
    writerOptions.outputBufferBytes = config_.writerBufferBytes;

    fq::io::FastqWriter writer(outputPath_, writerOptions);
    if (!writer.isOpen())
        throw std::runtime_error("Failed to open output file: " + outputPath_);

    try {
        size_t maxTokens = resolveMaxInFlightBatches(config_.maxInFlightBatches,
                                                     config_.memoryLimitBytes,
                                                     config_.batchCapacityBytes,
                                                     threadCount);
        finalStats.resolvedMaxInFlightBatches = maxTokens;

        std::shared_ptr<fq::io::FastqBatchPool> batchPool;
        switch (config_.memoryResourcePolicy) {
        case MemoryResourcePolicy::ObjectPool:
            batchPool = fq::io::createFastqBatchPool(maxTokens, maxTokens * 2);
            break;
        }
        if (!batchPool) {
            throw std::invalid_argument("Unsupported memory resource policy for processing pipeline.");
        }

        tbb::parallel_pipeline(
            maxTokens,

            tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
                tbb::filter_mode::serial_in_order,
                [reader, batchPool, this](
                    tbb::flow_control& fc) -> std::shared_ptr<fq::io::FastqBatch> {
                    auto batch = batchPool->acquire();
                    if (reader->nextBatch(*batch, config_.batchSize)) {
                        return batch;
                    }
                    fc.stop();
                    return nullptr;
                }) &

                tbb::make_filter<
                    std::shared_ptr<fq::io::FastqBatch>,
                    std::pair<std::shared_ptr<fq::io::FastqBatch>, ProcessingStatistics>>(
                    tbb::filter_mode::parallel,
                    [this](std::shared_ptr<fq::io::FastqBatch> batch) {
                        ProcessingStatistics batchStats;
                        this->processBatch(*batch, batchStats);
                        return std::make_pair(batch, batchStats);
                    }) &

                tbb::make_filter<
                    std::pair<std::shared_ptr<fq::io::FastqBatch>, ProcessingStatistics>,
                    void>(
                    tbb::filter_mode::serial_in_order,
                    [&writer, &finalStats](const std::pair<std::shared_ptr<fq::io::FastqBatch>,
                                                           ProcessingStatistics>& pair) {
                        const auto before = writer.totalUncompressedBytes();
                        writer.write(*pair.first);
                        const auto after = writer.totalUncompressedBytes();
                        finalStats.totalReads += pair.second.totalReads;
                        finalStats.passedReads += pair.second.passedReads;
                        finalStats.filteredReads += pair.second.filteredReads;
                        finalStats.modifiedReads += pair.second.modifiedReads;
                        finalStats.inputBytes += pair.second.inputBytes;
                        finalStats.outputBytes += (after - before);
                    }));

        auto endTime = std::chrono::steady_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        finalStats.elapsedMs = static_cast<uint64_t>(duration);
        finalStats.processingTimeMs = static_cast<double>(finalStats.elapsedMs);
        if (finalStats.elapsedMs > 0) {
            finalStats.throughputMbps =
                (static_cast<double>(finalStats.outputBytes) / 1024.0 / 1024.0) /
                (static_cast<double>(finalStats.elapsedMs) / 1000.0);
        }
    } catch (const std::exception& e) {
        fq::logging::error("TBB pipeline failed: {}", e.what());
        throw;
    }

    return finalStats;
}

}  // namespace fq::processing
