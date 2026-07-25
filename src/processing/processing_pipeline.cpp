/**
 * @file processing_pipeline.cpp
 * @brief 处理管道实现
 * @details 实现 FastQ 数据处理管道的串行和并行处理逻辑
 *
 */

#include "fqtools/processing/processing_pipeline_interface.h"

#include "fqtools/processing/interfaces.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

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

class Pipeline::Impl {
public:
    std::string inputPath_;                                            ///< 输入文件路径
    std::string outputPath_;                                           ///< 输出文件路径
    ProcessingOptions options_;                                        ///< 用户可见的处理选项
    std::vector<std::unique_ptr<ReadMutatorInterface>> mutators_;      ///< 数据修改器列表
    std::vector<std::unique_ptr<ReadPredicateInterface>> predicates_;  ///< 数据过滤器列表
    std::unique_ptr<fq::io::IReader> customReader_;                    ///< 自定义读取器（测试用）
    std::shared_ptr<fq::io::IWriter> customWriter_;                    ///< 自定义写入器（测试用）
    bool customReaderConfigured_ = false;

    auto run() -> ProcessingStatistics;

private:
    /// @brief 对一批数据应用所有修改器和过滤器
    auto processBatch(fq::io::FastqBatch& batch, ProcessingStatistics& stats) -> void;
};

Pipeline::Pipeline() : impl_(std::make_unique<Impl>()) {}
Pipeline::~Pipeline() = default;
Pipeline::Pipeline(Pipeline&&) noexcept = default;
auto Pipeline::operator=(Pipeline&&) noexcept -> Pipeline& = default;

void Pipeline::setInputPath(const std::string& inputPath) {
    impl_->inputPath_ = inputPath;
}

void Pipeline::setOutputPath(const std::string& outputPath) {
    impl_->outputPath_ = outputPath;
}

void Pipeline::setReader(std::unique_ptr<fq::io::IReader> reader) {
    impl_->customReaderConfigured_ = static_cast<bool>(reader);
    impl_->customReader_ = std::move(reader);
}

void Pipeline::setWriter(std::unique_ptr<fq::io::IWriter> writer) {
    impl_->customWriter_ = std::shared_ptr<fq::io::IWriter>(std::move(writer));
}

void Pipeline::setProcessingOptions(const ProcessingOptions& options) {
    impl_->options_ = options;
}

void Pipeline::addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) {
    impl_->mutators_.push_back(std::move(mutator));
}

void Pipeline::addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) {
    impl_->predicates_.push_back(std::move(predicate));
}

auto Pipeline::run() -> ProcessingStatistics {
    return impl_->run();
}

auto Pipeline::Impl::run() -> ProcessingStatistics {
    if (customReaderConfigured_ && !customReader_) {
        throw std::invalid_argument(
            "Pipeline: custom reader must be reset before rerunning");
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

auto Pipeline::Impl::processBatch(fq::io::FastqBatch& batch, ProcessingStatistics& stats)
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
