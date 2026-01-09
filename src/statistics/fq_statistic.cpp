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

#include "spdlog/spdlog.h"
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
    if (other.maxReadLength > this->maxReadLength) {
        this->maxReadLength = other.maxReadLength;
        if (this->maxReadLength > this->posQualityDist.size()) {
            this->posQualityDist.resize(this->maxReadLength, std::vector<uint64_t>(kMaxQual, 0));
            this->posBaseDist.resize(this->maxReadLength, std::vector<uint64_t>(kMaxBaseNum, 0));
        }
    }

    // Merge position stats
    size_t mergeLen = std::min(this->posQualityDist.size(), other.posQualityDist.size());
    for (size_t i = 0; i < mergeLen; ++i) {
        for (size_t j = 0; j < kMaxQual; ++j) {
            this->posQualityDist[i][j] += other.posQualityDist[i][j];
        }
        for (size_t j = 0; j < kMaxBaseNum; ++j) {
            this->posBaseDist[i][j] += other.posBaseDist[i][j];
        }
    }

    // If other is longer, copy the rest (should be handled by resize above? no, operator+= logic)
    // Actually, if we resized 'this', the new parts are 0, so we just add.
    if (other.posQualityDist.size() > mergeLen) {
        // Copy remaining parts from other
        for (size_t i = mergeLen; i < other.posQualityDist.size(); ++i) {
            this->posQualityDist[i] = other.posQualityDist[i];
            this->posBaseDist[i] = other.posBaseDist[i];
        }
    }

    return *this;
}

// Helper function
[[nodiscard]] static auto calculateErrorPerPosition(const std::vector<uint64_t>& posQualityDist, uint64_t readCount) -> double {
    if (readCount == 0) {
        return 0.0;
    }

    double errPerPos = 0.0;
    for (int i = 0; i < kMaxQual; ++i) {
        errPerPos += static_cast<double>(posQualityDist[i]) * std::pow(10.0, -0.1 * static_cast<double>(i));
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

    const size_t threadCount =
        std::max<size_t>(1, static_cast<size_t>(options_.threadCount));
    tbb::global_control globalLimit(tbb::global_control::max_allowed_parallelism, threadCount);

    size_t maxLiveTokens = std::max(static_cast<size_t>(1), threadCount * 2);
    if (options_.maxInFlightBatches > 0) {
        maxLiveTokens = options_.maxInFlightBatches;
    }
    if (options_.memoryLimitBytes > 0 && options_.batchCapacityBytes > 0) {
        const size_t cap =
            (options_.memoryLimitBytes * 7 / 10) / options_.batchCapacityBytes;
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
            [reader, batchPool, this](tbb::flow_control& fc) -> std::shared_ptr<fq::io::FastqBatch> {
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
                [](const std::shared_ptr<fq::io::FastqBatch>& batch) -> FqStatisticResult {
                    if (!batch) {
                        return FqStatisticResult();
                    }
                    // Assuming default qual offset 33 for now.
                    // TODO: Auto-detect quality system in Reader and pass here.
                    FqStatisticWorker worker(33);
                    return worker.calculateStats(*batch);
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
        throw std::runtime_error(
            "Failed to open output statistics file: " + options_.outputStatPath);
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
    writer << "#PhredQual\t" << 33 << "\n";
    writer << "#ReadNum\t" << result.readCount << "\n";
    writer << "#MaxReadLength\t" << result.maxReadLength
           << "\n";  // Changed from fixed ReadLength
    writer << "#BaseCount\t" << result.totalBases << "\n";

    constexpr int kQ20Threshold = 20;
    constexpr int kQ30Threshold = 30;
    uint64_t nQ20 = 0, nQ30 = 0;
    uint64_t nA = 0, nC = 0, nG = 0, nT = 0, nN = 0;

    // Iterate up to maxReadLength
    for (size_t i = 0; i < result.maxReadLength; ++i) {
        if (i >= result.posQualityDist.size()) {
            break;
        }

        for (int j = kQ20Threshold; j < kMaxQual; ++j) {
            nQ20 += result.posQualityDist[i][j];
        }
        for (int j = kQ30Threshold; j < kMaxQual; ++j) {
            nQ30 += result.posQualityDist[i][j];
        }

        nA += result.posBaseDist[i][0];
        nC += result.posBaseDist[i][1];
        nG += result.posBaseDist[i][2];
        nT += result.posBaseDist[i][3];
        nN += result.posBaseDist[i][4];
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
        if (i >= result.posBaseDist.size())
            break;

        writer << i + 1 << "\t";
        writer << result.posBaseDist[i][0] << "\t" << result.posBaseDist[i][1] << "\t"
               << result.posBaseDist[i][2] << "\t" << result.posBaseDist[i][3] << "\t"
               << result.posBaseDist[i][4] << "\t";

        uint64_t sumQual = 0;
        uint64_t countReadsAtPos = 0;  // Reads that cover this position

        for (int j = 0; j < kMaxQual; ++j) {
            sumQual += result.posQualityDist[i][j] * j;
            countReadsAtPos += result.posQualityDist[i][j];
        }

        if (countReadsAtPos > 0) {
            writer << static_cast<double>(sumQual) / static_cast<double>(countReadsAtPos)
                   << "\t";
            writer << calculateErrorPerPosition(result.posQualityDist[i], countReadsAtPos) << "\n";
        } else {
            writer << "0.0\t0.0\n";
        }
    }
}

}  // namespace fq::statistic
