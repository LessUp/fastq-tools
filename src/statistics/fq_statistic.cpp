/**
 * @file fq_statistic.cpp
 * @brief FASTQ 统计功能实现文件
 * @details 包含 FqStatistic 类及相关统计方法的实现，支持 TBB 并行统计。
 * @author FastQTools Team
 * @date 2025-08-01
 * @version 2.0
 * @copyright Copyright (c) 2025 FastQTools
 */

#include "statistics/fq_statistic.h"

#include "fqtools/io/fastq_batch_pool.h"
#include "fqtools/io/fastq_reader.h"
#include "fqtools/logging.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#include <numeric>
#include <vector>

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
    // No pre-inference needed in new architecture
}

void FastqStatisticCalculator::run() {
    fq::logging::info("Starting FASTQ statistics generation for '{}' using TBB pipeline (New IO).",
                      options_.inputFastqPath);

    FqStatisticResult finalResult;

    const size_t threadCount = std::max<size_t>(1, static_cast<size_t>(options_.threadCount));
    tbb::global_control globalLimit(tbb::global_control::max_allowed_parallelism, threadCount);

    size_t maxLiveTokens = std::max(static_cast<size_t>(1), threadCount * 2);
    if (options_.maxInFlightBatches > 0) {
        maxLiveTokens = options_.maxInFlightBatches;
    }
    if (options_.memoryLimitBytes > 0 && options_.batchCapacityBytes > 0) {
        const size_t cap = (options_.memoryLimitBytes * 7 / 10) / options_.batchCapacityBytes;
        if (cap > 0) {
            maxLiveTokens = std::min(maxLiveTokens, cap);
        }
    }
    maxLiveTokens = std::max(static_cast<size_t>(1), maxLiveTokens);

    fq::io::FastqReaderOptions readerOptions;
    readerOptions.readChunkBytes = options_.readChunkBytes;
    readerOptions.zlibBufferBytes = options_.zlibBufferBytes;
    readerOptions.maxBufferBytes = options_.batchCapacityBytes;

    // Shared reader for serial stage
    auto reader = std::make_shared<fq::io::FastqReader>(options_.inputFastqPath, readerOptions);
    if (!reader->isOpen()) {
        throw std::runtime_error("Failed to open input file: " + options_.inputFastqPath);
    }

    auto batchPool = fq::io::createFastqBatchPool(maxLiveTokens, maxLiveTokens * 2);

    tbb::parallel_pipeline(
        maxLiveTokens,
        // Stage 1: Input Filter (Serial)
        tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
            tbb::filter_mode::serial_in_order,
            [reader, batchPool, this](
                tbb::flow_control& fc) -> std::shared_ptr<fq::io::FastqBatch> {
                auto batch = batchPool->acquire();
                batch->buffer().reserve(options_.batchCapacityBytes);
                batch->records().reserve(static_cast<size_t>(options_.batchSize));
                if (reader->nextBatch(*batch, static_cast<size_t>(options_.batchSize))) {
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
                    FqStatisticWorker worker(options_.qualityEncoding);
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

    writer << std::fixed << std::setprecision(2);

    if (result.readCount == 0) {
        fq::logging::warn("No reads found in input file.");
        return;
    }

    std::string fqName = std::filesystem::path(options_.inputFastqPath).filename().string();

    // Infer info for header
    // Since we removed FileAttributes, we just output what we know.
    // QScoreType defaults to Sanger (33)

    writer << "#Name\t" << fqName << "\n";
    writer << "#PhredQual\t" << options_.qualityEncoding << "\n";
    writer << "#ReadNum\t" << result.readCount << "\n";
    writer << "#MaxReadLength\t" << result.maxReadLength << "\n";  // Changed from fixed ReadLength
    writer << "#BaseCount\t" << result.totalBases << "\n";

    constexpr int kQ20Threshold = 20;
    constexpr int kQ30Threshold = 30;
    uint64_t nQ20 = 0, nQ30 = 0;
    uint64_t nA = 0, nC = 0, nG = 0, nT = 0, nN = 0;

    // Iterate up to maxReadLength（扁平化布局：使用 qualityAt/baseAt 访问器）
    for (size_t i = 0; i < result.maxReadLength; ++i) {
        const uint64_t* qSlot = result.qualityAt(i);
        const uint64_t* bSlot = result.baseAt(i);

        for (int j = kQ20Threshold; j < kMaxQual; ++j) {
            nQ20 += qSlot[j];
        }
        for (int j = kQ30Threshold; j < kMaxQual; ++j) {
            nQ30 += qSlot[j];
        }

        nA += bSlot[0];
        nC += bSlot[1];
        nG += bSlot[2];
        nT += bSlot[3];
        nN += bSlot[4];
    }

    writer << "#Q20(>=20)\t" << nQ20 << "\t"
           << 100.0 * static_cast<double>(nQ20) / static_cast<double>(result.totalBases) << "%\n";
    writer << "#Q30(>=30)\t" << nQ30 << "\t"
           << 100.0 * static_cast<double>(nQ30) / static_cast<double>(result.totalBases) << "%\n";
    writer << "#A\t" << nA << "\t"
           << 100.0 * static_cast<double>(nA) / static_cast<double>(result.totalBases) << "%\n";
    writer << "#C\t" << nC << "\t"
           << 100.0 * static_cast<double>(nC) / static_cast<double>(result.totalBases) << "%\n";
    writer << "#G\t" << nG << "\t"
           << 100.0 * static_cast<double>(nG) / static_cast<double>(result.totalBases) << "%\n";
    writer << "#T\t" << nT << "\t"
           << 100.0 * static_cast<double>(nT) / static_cast<double>(result.totalBases) << "%\n";
    writer << "#N\t" << nN << "\t"
           << 100.0 * static_cast<double>(nN) / static_cast<double>(result.totalBases) << "%\n";
    writer << "#GC\t" << nG + nC << "\t"
           << 100.0 * static_cast<double>(nG + nC) / static_cast<double>(result.totalBases)
           << "%\n";

    writer << "#Pos\tA\tC\tG\tT\tN\tAvgQual\tErrRate\n";
    for (size_t i = 0; i < result.maxReadLength; ++i) {
        const uint64_t* bSlot = result.baseAt(i);
        const uint64_t* qSlot = result.qualityAt(i);

        writer << i + 1 << "\t";
        writer << bSlot[0] << "\t" << bSlot[1] << "\t" << bSlot[2] << "\t" << bSlot[3] << "\t"
               << bSlot[4] << "\t";

        uint64_t sumQual = 0;
        uint64_t countReadsAtPos = 0;

        for (int j = 0; j < kMaxQual; ++j) {
            sumQual += qSlot[j] * j;
            countReadsAtPos += qSlot[j];
        }

        if (countReadsAtPos > 0) {
            writer << static_cast<double>(sumQual) / static_cast<double>(countReadsAtPos) << "\t";
            writer << calculateErrorPerPosition(qSlot, countReadsAtPos) << "\n";
        } else {
            writer << "0.0\t0.0\n";
        }
    }
}

}  // namespace fq::statistic
