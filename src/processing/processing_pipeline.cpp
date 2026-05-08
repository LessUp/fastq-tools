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

#include "fqtools/logging.h"
#include "fqtools/processing/read_mutator_interface.h"
#include "fqtools/processing/read_predicate_interface.h"

#include <chrono>
#include <stdexcept>

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
    customWriter_ = std::shared_ptr<fq::io::IWriter>(std::move(writer));
}

void ProcessingPipeline::setProcessingOptions(const ProcessingOptions& options) {
    options_ = options;
}

void ProcessingPipeline::addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) {
    mutators_.push_back(std::move(mutator));
}

void ProcessingPipeline::addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) {
    predicates_.push_back(std::move(predicate));
}

auto ProcessingPipeline::run() -> ProcessingStatistics {
    try {
        ExecutionRuntimePlan runtimePlan;
        runtimePlan.inputPath = inputPath_;
        if (!outputPath_.empty()) {
            runtimePlan.outputPath = outputPath_;
        }
        runtimePlan.options = options_;

        runtime_.setCustomReader(std::move(customReader_));
        if (customWriter_) {
            runtime_.setCustomWriter(customWriter_);
        }

        auto startTime = std::chrono::steady_clock::now();
        auto stats = runtime_.run<ProcessingStatistics>(
            runtimePlan,
            [this](fq::io::FastqBatch& batch) {
                ProcessingStatistics partial;
                processBatch(batch, partial);
                return partial;
            },
            [](ProcessingStatistics& total, ProcessingStatistics partial) {
                total.totalReads += partial.totalReads;
                total.passedReads += partial.passedReads;
                total.filteredReads += partial.filteredReads;
                total.modifiedReads += partial.modifiedReads;
                total.inputBytes += partial.inputBytes;
                total.outputBytes += partial.outputBytes;
            },
            [](ProcessingStatistics& partial, std::uint64_t committedBytes) {
                partial.outputBytes += committedBytes;
            },
            ProcessingStatistics{});

        auto endTime = std::chrono::steady_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        stats.elapsedMs = static_cast<uint64_t>(duration);
        stats.processingTimeMs = static_cast<double>(stats.elapsedMs);
        if (stats.elapsedMs > 0) {
            stats.throughputMbps = (static_cast<double>(stats.outputBytes) / 1024.0 / 1024.0) /
                (static_cast<double>(stats.elapsedMs) / 1000.0);
        }
        return stats;
    } catch (const std::exception& e) {
        fq::logging::error("Error in processing pipeline: {}", e.what());
        throw;
    }
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
}  // namespace fq::processing
