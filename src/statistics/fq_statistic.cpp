/**
 * @file fq_statistic.cpp
 * @brief FASTQ 统计功能实现文件
 * @details 包含 FqStatistic 类及相关统计方法的实现，支持 TBB 并行统计。
 * @author FastQTools Team
 * @date 2025-08-01
 * @version 2.0
 * @copyright Copyright (c) 2025 FastQTools
 * @license MIT License
 */

#include "statistics/fq_statistic.h"

#include "fqtools/logging.h"
#include "fqtools/statistics/statistics_writer.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <memory>
#include <numeric>
#include <utility>
#include <vector>

#include "processing/execution_runtime.h"
#include "statistics/fq_statistic_worker.h"

namespace fq::statistic {

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

    auto processBatch(const fq::io::FastqBatch& batch) const -> result_type {
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

/**
 * @brief 统计结果累加操作符重载
 */
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

    for (const auto& [kmer, count] : other.headKmerCounts) {
        headKmerCounts[kmer] += count;
    }

    return *this;
}

FastqStatisticCalculator::FastqStatisticCalculator(StatisticOptions options)
    : options_(std::move(options)) {
    // 验证配置
    options_.processing.validate();
}

void FastqStatisticCalculator::run() {
    fq::logging::info("Starting FASTQ statistics generation for '{}' using execution runtime.",
                      options_.inputFastqPath);

    processing::ExecutionRuntime runtime;
    processing::ExecutionRuntimePlan plan;
    plan.inputPath = options_.inputFastqPath;
    plan.options = options_.processing;

    auto outcome = runtime.execute(plan,
                                   StatisticRuntimeAdapter{options_.qualityEncoding,
                                                           options_.signatureKmerSize,
                                                           options_.duplicateEstimateSampleModulo});
    auto finalResult = std::move(outcome.result);

    fq::logging::info("Execution runtime finished. Aggregated results from all batches.");
    writeResult(finalResult);
    fq::logging::info("Statistics report saved to '{}'", options_.outputStatPath);
}

void FastqStatisticCalculator::writeResult(const FqStatisticResult& result) {
    std::ofstream writer(options_.outputStatPath);
    if (!writer) {
        throw std::runtime_error("Failed to open output statistics file: " +
                                 options_.outputStatPath);
    }

    StatisticsWriterOptions writerOptions;
    writerOptions.inputFastqPath = options_.inputFastqPath;
    writerOptions.qualityEncoding = options_.qualityEncoding;
    writerOptions.duplicateEstimateSampleModulo = options_.duplicateEstimateSampleModulo;
    writerOptions.signatureReportPath = options_.signatureReportPath;
    writerOptions.maxReportedSignatures = options_.maxReportedSignatures;

    StatisticsWriter statsWriter(writerOptions);
    statsWriter.write(writer, result);

    if (!options_.signatureReportPath.empty()) {
        writeSignatureSidecar(result);
    }
}

void FastqStatisticCalculator::writeSignatureSidecar(const FqStatisticResult& result) const {
    std::ofstream writer(options_.signatureReportPath);
    if (!writer) {
        throw std::runtime_error("Failed to open signature report file: " +
                                 options_.signatureReportPath);
    }

    StatisticsWriterOptions writerOptions;
    writerOptions.duplicateEstimateSampleModulo = options_.duplicateEstimateSampleModulo;
    writerOptions.maxReportedSignatures = options_.maxReportedSignatures;

    StatisticsWriter statsWriter(writerOptions);
    statsWriter.writeSignature(writer, result);
}

}  // namespace fq::statistic
