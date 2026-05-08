/**
 * @file processing_pipeline.cpp
 * @brief 处理管道实现
 * @details 实现 FastQ 数据处理管道的串行和并行处理逻辑
 *
 * @author FastQTools Team
 * @date 2026
 * @version 2.0
 * @copyright Copyright (c) 2026 FastQTools
 * @license MIT License
 */

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

ProcessingPipeline::ProcessingPipeline() = default;
ProcessingPipeline::~ProcessingPipeline() = default;

void ProcessingPipeline::setInputPath(const std::string& inputPath) {
    inputPath_ = inputPath;
}

void ProcessingPipeline::setOutputPath(const std::string& outputPath) {
    outputPath_ = outputPath;
}

void ProcessingPipeline::setReader(std::unique_ptr<fq::io::IReader> reader) {
    customReader_ = std::move(reader);
}

void ProcessingPipeline::setWriter(std::unique_ptr<fq::io::IWriter> writer) {
    customWriter_ = std::move(writer);
}

void ProcessingPipeline::setProcessingOptions(const ProcessingOptions& options) {
    options_ = options;
    runtimePolicy_ = deriveRuntimePolicy(options);
}

void ProcessingPipeline::addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) {
    mutators_.push_back(std::move(mutator));
}

void ProcessingPipeline::addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) {
    predicates_.push_back(std::move(predicate));
}

auto ProcessingPipeline::run() -> ProcessingStatistics {
    const auto executionPlan = derivePipelineExecutionPlan(options_,
                                                           runtimePolicy_,
                                                           static_cast<bool>(customReader_),
                                                           static_cast<bool>(customWriter_));

    if (executionPlan.mode == PipelineExecutionMode::Parallel) {
        return processWithTBB(executionPlan);
    }
    return processSequential();
}

auto ProcessingPipeline::processSequential() -> ProcessingStatistics {
    ProcessingStatistics stats;

    try {
        // 创建或使用自定义 Reader
        std::unique_ptr<fq::io::FastqReader> fileReader;
        fq::io::IReader* reader = customReader_.get();

        if (!reader) {
            fq::io::FastqReaderOptions readerOptions;
            readerOptions.readChunkBytes = runtimePolicy_.readChunkBytes;
            readerOptions.zlibBufferBytes = runtimePolicy_.zlibBufferBytes;
            readerOptions.maxBufferBytes = runtimePolicy_.batchCapacityBytes;

            fileReader = std::make_unique<fq::io::FastqReader>(inputPath_, readerOptions);
            if (!fileReader->isOpen()) {
                throw std::runtime_error("Failed to open input file: " + inputPath_);
            }
            reader = fileReader.get();
        }

        // 创建或使用自定义 Writer
        std::unique_ptr<fq::io::FastqWriter> fileWriter;
        fq::io::IWriter* writer = customWriter_.get();

        if (!writer) {
            fq::io::FastqWriterOptions writerOptions;
            writerOptions.zlibBufferBytes = runtimePolicy_.zlibBufferBytes;
            writerOptions.outputBufferBytes = runtimePolicy_.writerBufferBytes;

            fileWriter = std::make_unique<fq::io::FastqWriter>(outputPath_, writerOptions);
            if (!fileWriter->isOpen()) {
                throw std::runtime_error("Failed to open output file: " + outputPath_);
            }
            writer = fileWriter.get();
        }

        fq::io::FastqBatch batch(runtimePolicy_.batchCapacityBytes, options_.batchSize);
        auto startTime = std::chrono::steady_clock::now();

        while (reader->nextBatch(batch)) {
            processBatch(batch, stats);
            writer->write(batch);
        }

        auto endTime = std::chrono::steady_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        stats.elapsedMs = static_cast<uint64_t>(duration);
        stats.processingTimeMs = static_cast<double>(stats.elapsedMs);

        // 注意：自定义 writer 可能没有 totalUncompressedBytes 方法
        if (fileWriter) {
            stats.outputBytes = fileWriter->totalUncompressedBytes();
        }
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

auto ProcessingPipeline::processWithTBB(const PipelineExecutionPlan& executionPlan)
    -> ProcessingStatistics {
    ProcessingStatistics finalStats;
    auto startTime = std::chrono::steady_clock::now();

    tbb::global_control globalLimit(tbb::global_control::max_allowed_parallelism,
                                    executionPlan.threadCount);

    fq::io::FastqReaderOptions readerOptions;
    readerOptions.readChunkBytes = runtimePolicy_.readChunkBytes;
    readerOptions.zlibBufferBytes = runtimePolicy_.zlibBufferBytes;
    readerOptions.maxBufferBytes = runtimePolicy_.batchCapacityBytes;

    auto reader = std::make_shared<fq::io::FastqReader>(inputPath_, readerOptions);
    if (!reader->isOpen())
        throw std::runtime_error("Failed to open input file: " + inputPath_);

    fq::io::FastqWriterOptions writerOptions;
    writerOptions.zlibBufferBytes = runtimePolicy_.zlibBufferBytes;
    writerOptions.outputBufferBytes = runtimePolicy_.writerBufferBytes;

    fq::io::FastqWriter writer(outputPath_, writerOptions);
    if (!writer.isOpen())
        throw std::runtime_error("Failed to open output file: " + outputPath_);

    try {
        // 计算最大并行批次数
        const size_t maxTokens = executionPlan.maxLiveTokens;

        // 使用 ObjectPool 作为内存资源策略
        auto batchPool = fq::io::createFastqBatchPool(maxTokens, maxTokens * 2);

        tbb::parallel_pipeline(
            maxTokens,

            tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
                tbb::filter_mode::serial_in_order,
                [reader, batchPool, this](
                    tbb::flow_control& fc) -> std::shared_ptr<fq::io::FastqBatch> {
                    auto batch = batchPool->acquire();
                    if (reader->nextBatch(*batch, options_.batchSize)) {
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
