/**
 * @file fq_statistic.cpp
 * @brief FASTQ 统计计算器实现
 * @details Calculator::Impl 内部走 ExecutionRuntime + StatisticRuntimeAdapter，
 *          合并了原 FastqStatisticCalculator 的职责（消除中间类）。
 */

#include "statistics/fq_statistic.h"

#include "fqtools/error/error.h"
#include "fqtools/logging.h"
#include "fqtools/statistics/statistics_writer.h"

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <unistd.h>

#include "processing/execution_runtime.h"
#include "statistics/fq_statistic_worker.h"

namespace fq::statistics {

namespace {

class StatisticRuntimeAdapter {
public:
    using result_type = FqStatisticResult;

    StatisticRuntimeAdapter(int qualityEncoding,
                            size_t signatureKmerSize,
                            size_t duplicateEstimateSampleModulo)
        : qualityEncoding_(qualityEncoding),
          signatureKmerSize_(signatureKmerSize),
          duplicateEstimateSampleModulo_(duplicateEstimateSampleModulo) {}

    [[nodiscard]] auto makeResult() const -> result_type {
        return {};
    }

    [[nodiscard]] auto processBatch(const fq::io::FastqBatch& batch) const -> result_type {
        FqStatisticWorker worker(
            qualityEncoding_, signatureKmerSize_, duplicateEstimateSampleModulo_);
        return worker.calculateStats(batch);
    }

    void afterCommit(result_type& /*unused*/, std::uint64_t /*unused*/) const {}

    void merge(result_type& total, const result_type& partial) const {
        total += partial;
    }

private:
    int qualityEncoding_;
    size_t signatureKmerSize_;
    size_t duplicateEstimateSampleModulo_;
};

}  // namespace

auto FqStatisticResult::operator+=(const FqStatisticResult& other) -> FqStatisticResult& {
    this->readCount += other.readCount;
    this->totalBases += other.totalBases;
    this->duplicateSampledReads += other.duplicateSampledReads;

    // 确保 this 能容纳 other 的最大长度
    if (other.maxReadLength > this->maxReadLength) {
        ensureCapacity(other.maxReadLength);
    }

    // 向量化友好的合并：使用 __restrict__ 指针消除别名分析障碍，
    // 让编译器 -O3 -ftree-vectorize 能生成 AVX2 SIMD 指令
    {
        const size_t n = std::min(posQualityDist.size(), other.posQualityDist.size());
        uint64_t* __restrict__ dst = posQualityDist.data();
        const uint64_t* __restrict__ src = other.posQualityDist.data();
        for (size_t i = 0; i < n; ++i) {
            dst[i] += src[i];
        }
    }

    {
        const size_t n = std::min(posBaseDist.size(), other.posBaseDist.size());
        uint64_t* __restrict__ dst = posBaseDist.data();
        const uint64_t* __restrict__ src = other.posBaseDist.data();
        for (size_t i = 0; i < n; ++i) {
            dst[i] += src[i];
        }
    }

    for (const auto& [hashValue, count] : other.sampledSequenceHashes) {
        auto& current = sampledSequenceHashes[hashValue];
        if (current > 0 && count > 0) {
            ++duplicateSampledReads;
        }
        current += count;
    }

    // 合并每批的有界 sketch 到全局有界 sketch：worker 侧每批限 64 个 key，
    // 但高多样性数据下批间几乎不重复，无界合并会随批数线性膨胀
    // （10 万批 → 数百万 map 节点）。允许临时超界一倍，超限后按计数保留 top-K——
    // 报告只取 top-N（N << K），剪枝不影响结果。
    constexpr size_t kMaxHeadKmerEntries = 4096;
    for (const auto& [kmer, count] : other.headKmerCounts) {
        headKmerCounts[kmer] += count;
    }
    if (headKmerCounts.size() > kMaxHeadKmerEntries * 2) {
        std::vector<std::pair<std::string, uint64_t>> entries(headKmerCounts.begin(),
                                                              headKmerCounts.end());
        std::partial_sort(entries.begin(),
                          entries.begin() + kMaxHeadKmerEntries,
                          entries.end(),
                          [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });
        headKmerCounts.clear();
        for (size_t i = 0; i < kMaxHeadKmerEntries; ++i) {
            headKmerCounts.emplace(std::move(entries[i].first), entries[i].second);
        }
    }

    return *this;
}

class Calculator::Impl {
public:
    explicit Impl(StatisticOptions options) : options_(std::move(options)) {
        options_.processing.validate();
    }

    void run() {
        fq::logging::info("Starting FASTQ statistics generation for '{}' using execution runtime.",
                          options_.inputFastqPath);

        processing::ExecutionRuntime runtime;
        processing::ExecutionRuntimeRequest plan;
        plan.inputPath = options_.inputFastqPath;
        plan.options = options_.processing;

        auto outcome =
            runtime.execute(plan,
                            StatisticRuntimeAdapter{options_.qualityEncoding,
                                                    options_.signatureKmerSize,
                                                    options_.duplicateEstimateSampleModulo});
        auto finalResult = std::move(outcome.result);

        fq::logging::info("Execution runtime finished. Aggregated results from all batches.");
        writeResult(finalResult);
        fq::logging::info("Statistics report saved to '{}'", options_.outputStatPath);
    }

private:
    // 先写同目录临时文件、校验流状态、再原子 rename 发布——
    // 与 FastqWriter 的发布协议一致：磁盘满等写失败抛 IOError，绝不产出半截报告
    static void writeAtomically(const std::string& target,
                                const std::function<void(std::ofstream&)>& writeBody) {
        // 唯一后缀（pid+counter）：与 FastqWriter 临时文件命名一致，
        // 避免固定 .tmp 在多进程写同一目标时互相覆盖/发布对方的半成品
        static std::atomic<std::uint64_t> counter{0};
        const auto tmpPath = target + ".tmp-" + std::to_string(::getpid()) + "-" +
            std::to_string(counter.fetch_add(1, std::memory_order_relaxed));
        // 任意阶段抛异常（writeBody/flush/rename）都清理残留临时文件，避免磁盘遗留半截报告
        try {
            {
                std::ofstream writer(tmpPath);
                if (!writer) {
                    throw fq::error::IOError(tmpPath, errno);
                }
                writeBody(writer);
                writer.flush();
                if (!writer.good()) {
                    throw fq::error::IOError(target, EIO);
                }
            }  // writer 此处析构关闭，确保数据落盘后再 rename
            std::error_code renameError;
            std::filesystem::rename(tmpPath, target, renameError);
            if (renameError) {
                throw fq::error::IOError(target, renameError.value());
            }
        } catch (...) {
            std::error_code removeError;
            std::filesystem::remove(tmpPath, removeError);
            throw;
        }
    }

    void writeResult(const FqStatisticResult& result) {
        StatisticsWriterOptions writerOptions;
        writerOptions.inputFastqPath = options_.inputFastqPath;
        writerOptions.qualityEncoding = options_.qualityEncoding;
        writerOptions.duplicateEstimateSampleModulo = options_.duplicateEstimateSampleModulo;
        writerOptions.signatureReportPath = options_.signatureReportPath;
        writerOptions.maxReportedSignatures = options_.maxReportedSignatures;

        StatisticsWriter statsWriter(writerOptions);
        writeAtomically(options_.outputStatPath,
                        [&](std::ofstream& writer) { statsWriter.write(writer, result); });

        if (!options_.signatureReportPath.empty()) {
            writeSignatureSidecar(result);
        }
    }

    void writeSignatureSidecar(const FqStatisticResult& result) const {
        StatisticsWriterOptions writerOptions;
        writerOptions.duplicateEstimateSampleModulo = options_.duplicateEstimateSampleModulo;
        writerOptions.maxReportedSignatures = options_.maxReportedSignatures;

        StatisticsWriter statsWriter(writerOptions);
        writeAtomically(options_.signatureReportPath,
                        [&](std::ofstream& writer) { statsWriter.writeSignature(writer, result); });
    }

    StatisticOptions options_;
};

Calculator::Calculator(StatisticOptions options)
    : impl_(std::make_unique<Impl>(std::move(options))) {}
Calculator::~Calculator() = default;
Calculator::Calculator(Calculator&&) noexcept = default;
auto Calculator::operator=(Calculator&&) noexcept -> Calculator& = default;

void Calculator::run() {
    impl_->run();
}

}  // namespace fq::statistics
