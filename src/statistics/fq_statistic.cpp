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

#include "fqtools/io/fastq_batch_pool.h"
#include "fqtools/io/fastq_reader.h"
#include "fqtools/logging.h"
#include "fqtools/statistics/statistics_writer.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

#include "processing/resolved_runtime_config.h"
#include "statistics/fq_statistic_worker.h"
#include <tbb/global_control.h>
#include <tbb/parallel_pipeline.h>

namespace fq::statistic {

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

// Helper function: 计算单个位置的错误率
// qualSlot 指向该位置的 kMaxQual 个质量分数槽位（扁平化布局）
[[nodiscard]] static auto calculateErrorPerPosition(const uint64_t* qualSlot,
                                                    uint64_t readCount) -> double {
    if (readCount == 0) {
        return 0.0;
    }

    double errPerPos = 0.0;
    for (int i = 0; i < kMaxQual; ++i) {
        errPerPos +=
            static_cast<double>(qualSlot[i]) * std::pow(10.0, -0.1 * static_cast<double>(i));
    }
    return errPerPos / static_cast<double>(readCount);
}

FastqStatisticCalculator::FastqStatisticCalculator(const StatisticOptions& options)
    : options_(options) {
    // 验证配置
    options_.processing.validate();
}

void FastqStatisticCalculator::run() {
    fq::logging::info("Starting FASTQ statistics generation for '{}' using TBB pipeline.",
                      options_.inputFastqPath);

    const auto runtimeConfig = processing::resolveRuntimeConfig(options_.processing);

    FqStatisticResult finalResult;

    const size_t threadCount = std::max<size_t>(1, options_.processing.threadCount);
    tbb::global_control globalLimit(tbb::global_control::max_allowed_parallelism, threadCount);

    fq::io::FastqReaderOptions readerOptions;
    readerOptions.readChunkBytes = runtimeConfig.readChunkBytes;
    readerOptions.zlibBufferBytes = runtimeConfig.zlibBufferBytes;
    readerOptions.maxBufferBytes = runtimeConfig.batchCapacityBytes;

    auto reader = std::make_shared<fq::io::FastqReader>(options_.inputFastqPath, readerOptions);
    if (!reader->isOpen()) {
        throw std::runtime_error("Failed to open input file: " + options_.inputFastqPath);
    }

    auto batchPool =
        fq::io::createFastqBatchPool(runtimeConfig.maxLiveTokens, runtimeConfig.maxLiveTokens * 2);

    tbb::parallel_pipeline(
        runtimeConfig.maxLiveTokens,
        tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
            tbb::filter_mode::serial_in_order,
            [reader, batchPool, this, &runtimeConfig](
                tbb::flow_control& fc) -> std::shared_ptr<fq::io::FastqBatch> {
                auto batch = batchPool->acquire();
                batch->buffer().reserve(runtimeConfig.batchCapacityBytes);
                batch->records().reserve(options_.processing.batchSize);
                if (reader->nextBatch(*batch, options_.processing.batchSize)) {
                    return batch;
                } else {
                    fc.stop();
                    return nullptr;
                }
            }) &
            tbb::make_filter<std::shared_ptr<fq::io::FastqBatch>, FqStatisticResult>(
                tbb::filter_mode::parallel,
                [this](const std::shared_ptr<fq::io::FastqBatch>& batch) -> FqStatisticResult {
                    if (!batch) {
                        return FqStatisticResult();
                    }
                    FqStatisticWorker worker(options_.qualityEncoding,
                                             options_.signatureKmerSize,
                                             options_.duplicateEstimateSampleModulo);
                    FqStatisticResult result;
                    result.ensureCapacity(150);
                    result = worker.calculateStats(*batch);
                    return result;
                }) &
            tbb::make_filter<FqStatisticResult, void>(
                tbb::filter_mode::serial_in_order,
                [&finalResult](const FqStatisticResult& partialResult) {
                    finalResult += partialResult;
                }));

    fq::logging::info("TBB pipeline finished. Aggregated results from all batches.");
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

void FastqStatisticCalculator::writeSignatureSidecar(const FqStatisticResult& result) {
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
