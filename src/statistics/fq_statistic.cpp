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

#include "processing/runtime_policy.h"
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

    // 生成内部配置
    const auto runtimePolicy = processing::deriveRuntimePolicy(options_.processing);

    FqStatisticResult finalResult;

    const size_t threadCount = std::max<size_t>(1, options_.processing.threadCount);
    tbb::global_control globalLimit(tbb::global_control::max_allowed_parallelism, threadCount);

    fq::io::FastqReaderOptions readerOptions;
    readerOptions.readChunkBytes = runtimePolicy.readChunkBytes;
    readerOptions.zlibBufferBytes = runtimePolicy.zlibBufferBytes;
    readerOptions.maxBufferBytes = runtimePolicy.batchCapacityBytes;

    // Shared reader for serial stage
    auto reader = std::make_shared<fq::io::FastqReader>(options_.inputFastqPath, readerOptions);
    if (!reader->isOpen()) {
        throw std::runtime_error("Failed to open input file: " + options_.inputFastqPath);
    }

    // 使用 ObjectPool 作为内存资源策略
    auto batchPool =
        fq::io::createFastqBatchPool(runtimePolicy.maxLiveTokens, runtimePolicy.maxLiveTokens * 2);

    tbb::parallel_pipeline(
        runtimePolicy.maxLiveTokens,
        // Stage 1: Input Filter (Serial)
        tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
            tbb::filter_mode::serial_in_order,
            [reader, batchPool, this, &runtimePolicy](
                tbb::flow_control& fc) -> std::shared_ptr<fq::io::FastqBatch> {
                auto batch = batchPool->acquire();
                batch->buffer().reserve(runtimePolicy.batchCapacityBytes);
                batch->records().reserve(options_.processing.batchSize);
                if (reader->nextBatch(*batch, options_.processing.batchSize)) {
                    return batch;
                } else {
                    fc.stop();
                    return nullptr;
                }
            }) &
            // Stage 2: Processing Filter (Parallel)
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
                    // 预分配典型 Illumina read length (150bp) 的容量，
                    // 避免 ensureCapacity 在处理每条 read 时反复 resize
                    result.ensureCapacity(150);
                    result = worker.calculateStats(*batch);
                    return result;
                }) &
            // Stage 3: Aggregation Filter (Serial)
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
