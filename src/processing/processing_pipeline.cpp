/**
 * @file processing_pipeline.cpp
 * @brief 处理管道实现
 * @details Pipeline::Impl 直接实现 ExecutionRuntime 的 Adapter 契约，
 *          无需 FilterRuntimeAdapter / std::function 中间层。
 */

#include "fqtools/processing/interfaces.h"
#include "fqtools/processing/processing_pipeline_interface.h"

#include <chrono>
#include <cstdint>
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

}  // namespace

class Pipeline::Impl {
public:
    using result_type = ProcessingStatistics;

    std::string inputPath_;
    std::string outputPath_;
    ProcessingOptions options_;
    std::vector<std::unique_ptr<ReadMutatorInterface>> mutators_;
    std::vector<std::unique_ptr<ReadPredicateInterface>> predicates_;
    std::unique_ptr<fq::io::IReader> customReader_;
    std::shared_ptr<fq::io::IWriter> customWriter_;
    bool customReaderConfigured_ = false;

    auto run() -> ProcessingStatistics;

    // ExecutionRuntime::execute 的 Adapter 契约：直接成员函数，不经 std::function 间接。
    [[nodiscard]] auto makeResult() const -> result_type {
        return {};
    }

    auto processBatch(fq::io::FastqBatch& batch) -> result_type {
        ProcessingStatistics partial;
        applyOperations(batch, partial);
        return partial;
    }

    void afterCommit(result_type& partial, std::uint64_t committedBytes) const {
        partial.outputBytes += committedBytes;
    }

    void merge(result_type& total, result_type partial) const {
        total.totalReads += partial.totalReads;
        total.passedReads += partial.passedReads;
        total.filteredReads += partial.filteredReads;
        total.modifiedReads += partial.modifiedReads;
        total.inputBytes += partial.inputBytes;
        total.outputBytes += partial.outputBytes;
    }

private:
    /// @brief 对一批数据应用所有修改器和过滤器
    auto applyOperations(fq::io::FastqBatch& batch, ProcessingStatistics& stats) -> void;
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
        throw std::invalid_argument("Pipeline: custom reader must be reset before rerunning");
    }

    ExecutionRuntimeRequest runtimePlan;
    runtimePlan.inputPath = inputPath_;
    if (!outputPath_.empty()) {
        runtimePlan.outputPath = outputPath_;
    }
    runtimePlan.options = options_;

    ExecutionRuntime runtime(std::move(customReader_), customWriter_);

    auto startTime = std::chrono::steady_clock::now();
    auto outcome = runtime.execute(runtimePlan, *this);
    auto stats = std::move(outcome.result);

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

auto Pipeline::Impl::applyOperations(fq::io::FastqBatch& batch, ProcessingStatistics& stats)
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
