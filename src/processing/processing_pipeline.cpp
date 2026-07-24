/**
 * @file processing_pipeline.cpp
 * @brief 处理管道实现
 * @details 实现 FastQ 数据处理管道的串行和并行处理逻辑
 *
 * @author LessUp
 *
 * SPDX-License-Identifier: MIT
 */

#include "processing/processing_pipeline.h"

#include "fqtools/processing/interfaces.h"

#include <chrono>
#include <functional>
#include <stdexcept>

#include "processing/execution_runtime.h"

namespace fq::processing {

namespace {

[[nodiscard]] auto recordsEqual(const fq::io::FastqRecord& lhs, const fq::io::FastqRecord& rhs)
    -> bool {
    return lhs.id == rhs.id && lhs.comment == rhs.comment && lhs.seq == rhs.seq &&
        lhs.qual == rhs.qual && lhs.plus == rhs.plus;
}

class FilterRuntimeAdapter {
public:
    using result_type = ProcessingStatistics;

    using ProcessBatchFn = std::function<result_type(fq::io::FastqBatch&)>;
    using AfterCommitFn = std::function<void(result_type&, std::uint64_t)>;
    using MergeFn = std::function<void(result_type&, result_type)>;

    FilterRuntimeAdapter(ProcessBatchFn processBatch, AfterCommitFn afterCommit, MergeFn merge)
        : processBatch_(std::move(processBatch)),
          afterCommit_(std::move(afterCommit)),
          merge_(std::move(merge)) {}

    [[nodiscard]] auto makeResult() const -> result_type {
        return {};
    }

    auto processBatch(fq::io::FastqBatch& batch) -> result_type {
        return processBatch_(batch);
    }

    void afterCommit(result_type& partial, std::uint64_t committedBytes) const {
        afterCommit_(partial, committedBytes);
    }

    void merge(result_type& total, result_type partial) const {
        merge_(total, partial);
    }

private:
    ProcessBatchFn processBatch_;
    AfterCommitFn afterCommit_;
    MergeFn merge_;
};

}  // namespace

ProcessingPipeline::ProcessingPipeline() = default;
ProcessingPipeline::~ProcessingPipeline() = default;

void ProcessingPipeline::setInputPath(const std::string& inputPath) {
    inputPath_ = inputPath;
}

void ProcessingPipeline::setOutputPath(const std::string& outputPath) {
    outputPath_ = outputPath;
}

void ProcessingPipeline::setReader(std::unique_ptr<fq::io::IReader> reader) {
    customReaderConfigured_ = static_cast<bool>(reader);
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
    if (customReaderConfigured_ && !customReader_) {
        throw std::invalid_argument(
            "ProcessingPipeline: custom reader must be reset before rerunning");
    }

    ExecutionRuntimeRequest runtimePlan;
    runtimePlan.inputPath = inputPath_;
    if (!outputPath_.empty()) {
        runtimePlan.outputPath = outputPath_;
    }
    runtimePlan.options = options_;

    ExecutionRuntime runtime(std::move(customReader_), customWriter_);

    auto adapter =
        FilterRuntimeAdapter{[this](fq::io::FastqBatch& batch) {
                                 ProcessingStatistics partial;
                                 processBatch(batch, partial);
                                 return partial;
                             },
                             [](ProcessingStatistics& partial, std::uint64_t committedBytes) {
                                 partial.outputBytes += committedBytes;
                             },
                             [](ProcessingStatistics& total, ProcessingStatistics partial) {
                                 total.totalReads += partial.totalReads;
                                 total.passedReads += partial.passedReads;
                                 total.filteredReads += partial.filteredReads;
                                 total.modifiedReads += partial.modifiedReads;
                                 total.inputBytes += partial.inputBytes;
                                 total.outputBytes += partial.outputBytes;
                             }};

    auto startTime = std::chrono::steady_clock::now();
    auto outcome = runtime.execute(runtimePlan, adapter);
    auto stats = outcome.result;

    auto endTime = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    stats.elapsedMs = static_cast<uint64_t>(duration);
    if (stats.elapsedMs > 0) {
        stats.throughputMbps = (static_cast<double>(stats.outputBytes) / 1024.0 / 1024.0) /
            (static_cast<double>(stats.elapsedMs) / 1000.0);
    }
    return stats;
}

auto ProcessingPipeline::processBatch(fq::io::FastqBatch& batch, ProcessingStatistics& stats)
    -> void {
    stats.inputBytes += batch.buffer().size();
    auto& records = batch.records();
    const size_t totalInBatch = records.size();
    size_t passedCount = 0;
    const bool hasPredicates = !predicates_.empty();
    const bool hasMutators = !mutators_.empty();

    size_t modifiedCount = 0;

    for (size_t i = 0; i < totalInBatch; ++i) {
        auto& read = records[i];

        const auto originalRead = read;
        for (const auto& mutator : mutators_) {
            mutator->process(read);
        }

        bool passed = !read.empty();
        if (passed && hasPredicates) {
            for (const auto& predicate : predicates_) {
                if (!predicate->evaluate(read)) {
                    passed = false;
                    break;
                }
            }
        }

        if (passed && hasMutators && !recordsEqual(originalRead, read)) {
            ++modifiedCount;
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
}
}  // namespace fq::processing
