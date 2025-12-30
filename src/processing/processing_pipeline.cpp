#include "processing/processing_pipeline.h"

#include "fqtools/io/fastq_batch_pool.h"
#include "fqtools/io/fastq_reader.h"
#include "fqtools/io/fastq_writer.h"
#include "fqtools/processing/read_mutator_interface.h"
#include "fqtools/processing/read_predicate_interface.h"

#include <algorithm>
#include <stdexcept>

#include <spdlog/spdlog.h>
#include <tbb/global_control.h>
#include <tbb/parallel_pipeline.h>

namespace fq::processing {

SequentialProcessingPipeline::SequentialProcessingPipeline() = default;
SequentialProcessingPipeline::~SequentialProcessingPipeline() = default;

void SequentialProcessingPipeline::setInputPath(const std::string& inputPath) {
    inputPath_ = inputPath;
}
void SequentialProcessingPipeline::setOutputPath(const std::string& outputPath) {
    outputPath_ = outputPath;
}
void SequentialProcessingPipeline::setProcessingConfig(const ProcessingConfig& config) {
    config_ = config;
}
void SequentialProcessingPipeline::addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) {
    mutators_.push_back(std::move(mutator));
}
void SequentialProcessingPipeline::addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) {
    predicates_.push_back(std::move(predicate));
}

auto SequentialProcessingPipeline::run() -> ProcessingStatistics {
    if (config_.threadCount > 1) {
        return processWithTBB();
    } else {
        return processSequential();
    }
}

auto SequentialProcessingPipeline::processSequential() -> ProcessingStatistics {
    ProcessingStatistics stats;

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
        spdlog::error("Error in sequential processing: {}", e.what());
        throw;
    }

    return stats;
}

auto SequentialProcessingPipeline::processBatch(fq::io::FastqBatch& batch,
                                                ProcessingStatistics& stats) -> bool {
    stats.inputBytes += batch.buffer().size();
    auto& records = batch.records();
    size_t passedCount = 0;

    for (size_t i = 0; i < records.size(); ++i) {
        auto& read = records[i];
        stats.totalReads++;

        bool passed = true;
        for (const auto& predicate : predicates_) {
            if (!predicate->evaluate(read)) {
                passed = false;
                break;
            }
        }

        if (passed) {
            for (const auto& mutator : mutators_) {
                mutator->process(read);
            }

            if (read.empty()) {
                passed = false;
            }
        }

        if (passed) {
            if (passedCount != i) {
                records[passedCount] = read;
            }
            passedCount++;
        } else {
            stats.filteredReads++;
        }
    }

    stats.passedReads += passedCount;
    records.resize(passedCount);

    return true;
}

auto SequentialProcessingPipeline::processWithTBB() -> ProcessingStatistics {
    ProcessingStatistics finalStats;
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
        size_t maxTokens = std::max(static_cast<size_t>(4), threadCount * 2);
        if (config_.maxInFlightBatches > 0) {
            maxTokens = config_.maxInFlightBatches;
        }
        if (config_.memoryLimitBytes > 0 && config_.batchCapacityBytes > 0) {
            const size_t cap =
                (config_.memoryLimitBytes * 7 / 10) / config_.batchCapacityBytes;
            if (cap > 0) {
                maxTokens = std::min(maxTokens, cap);
            }
        }
        maxTokens = std::max(static_cast<size_t>(1), maxTokens);

        // 创建 FastqBatch 对象池，预分配 maxTokens 个对象
        auto batchPool = fq::io::createFastqBatchPool(maxTokens, maxTokens * 2);

        tbb::parallel_pipeline(
            maxTokens,

            tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
                tbb::filter_mode::serial_in_order,
                [reader, batchPool, this](tbb::flow_control& fc) -> std::shared_ptr<fq::io::FastqBatch> {
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
        spdlog::error("TBB pipeline failed: {}", e.what());
        throw;
    }

    return finalStats;
}

}  // namespace fq::processing
