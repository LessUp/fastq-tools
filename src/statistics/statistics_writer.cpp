/**
 * @file statistics_writer.cpp
 * @brief 统计结果输出器实现
 */

#include "fqtools/statistics/statistics_writer.h"

#include "fqtools/common/common.h"
#include "fqtools/logging.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <map>
#include <vector>

#include "statistics/fq_statistic.h"

namespace fq::statistic {

namespace {

// 与 FqStatisticResult 一致的最大质量值
constexpr int kWriterMaxQual = 128;

auto calculateErrorPerPosition(const uint64_t* qualSlot, uint64_t totalBases) -> double {
    if (totalBases == 0) {
        return 0.0;
    }
    double sumProb = 0.0;
    for (int q = 0; q < kWriterMaxQual; ++q) {
        // Q10 = 10% error, Q20 = 1%, Q30 = 0.1%, etc.
        double errorProb = std::pow(10.0, -static_cast<double>(q) / 10.0);
        sumProb += static_cast<double>(qualSlot[q]) * errorProb;
    }
    return 100.0 * sumProb / static_cast<double>(totalBases);
}

auto sortedTopEntries(const std::map<std::string, uint64_t>& counts,
                      size_t limit) -> std::vector<std::pair<std::string, uint64_t>> {
    std::vector<std::pair<std::string, uint64_t>> entries(counts.begin(), counts.end());
    std::sort(entries.begin(), entries.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.second != rhs.second) {
            return lhs.second > rhs.second;
        }
        return lhs.first < rhs.first;
    });
    if (entries.size() > limit) {
        entries.resize(limit);
    }
    return entries;
}

auto estimateDuplicates(uint64_t duplicateSampledReads, size_t sampleModulo) -> uint64_t {
    return duplicateSampledReads * std::max<size_t>(1, sampleModulo);
}

auto memoryPolicyName(fq::processing::MemoryResourcePolicy policy) -> const char* {
    switch (policy) {
        case fq::processing::MemoryResourcePolicy::ObjectPool:
            return "objectPool";
    }
    return "unknown";
}

}  // namespace

StatisticsWriter::StatisticsWriter(const StatisticsWriterOptions& options) : options_(options) {}

void StatisticsWriter::write(std::ostream& os, const FqStatisticResult& result) {
    os << std::fixed << std::setprecision(2);

    if (result.readCount == 0) {
        fq::logging::warn("No reads found in input file.");
        return;
    }

    std::string fqName = std::filesystem::path(options_.inputFastqPath).filename().string();

    // Header
    os << "#Name\t" << fqName << "\n";
    os << "#PhredQual\t" << options_.qualityEncoding << "\n";
    os << "#ReadNum\t" << result.readCount << "\n";

    const auto duplicateEstimate =
        estimateDuplicates(result.duplicateSampledReads, options_.duplicateEstimateSampleModulo);
    os << "#DuplicateEstimate\t" << duplicateEstimate << "\n";
    os << "#DuplicateEstimateRate\t"
       << 100.0 * static_cast<double>(duplicateEstimate) / static_cast<double>(result.readCount)
       << "%\n";

    if (options_.allocationTelemetryEnabled) {
        os << "#MemoryPolicy\t" << memoryPolicyName(options_.memoryResourcePolicy) << "\n";
        os << "#MaxInFlightBatches\t"
           << fq::common::resolveMaxInFlightBatches(
                  options_.maxInFlightBatches,
                  options_.memoryLimitBytes,
                  options_.batchCapacityBytes,
                  std::max<size_t>(1, static_cast<size_t>(options_.threadCount)))
           << "\n";
    }

    os << "#MaxReadLength\t" << result.maxReadLength << "\n";
    os << "#BaseCount\t" << result.totalBases << "\n";

    // Q20/Q30 和碱基统计
    constexpr int kQ20Threshold = 20;
    constexpr int kQ30Threshold = 30;
    uint64_t nQ20 = 0, nQ30 = 0;
    uint64_t nA = 0, nC = 0, nG = 0, nT = 0, nN = 0;

    for (size_t i = 0; i < result.maxReadLength; ++i) {
        const uint64_t* qSlot = result.qualityAt(i);
        const uint64_t* bSlot = result.baseAt(i);

        for (int j = kQ20Threshold; j < kWriterMaxQual; ++j) {
            nQ20 += qSlot[j];
        }
        for (int j = kQ30Threshold; j < kWriterMaxQual; ++j) {
            nQ30 += qSlot[j];
        }

        nA += bSlot[0];
        nC += bSlot[1];
        nG += bSlot[2];
        nT += bSlot[3];
        nN += bSlot[4];
    }

    os << "#Q20(>=20)\t" << nQ20 << "\t"
       << 100.0 * static_cast<double>(nQ20) / static_cast<double>(result.totalBases) << "%\n";
    os << "#Q30(>=30)\t" << nQ30 << "\t"
       << 100.0 * static_cast<double>(nQ30) / static_cast<double>(result.totalBases) << "%\n";
    os << "#A\t" << nA << "\t"
       << 100.0 * static_cast<double>(nA) / static_cast<double>(result.totalBases) << "%\n";
    os << "#C\t" << nC << "\t"
       << 100.0 * static_cast<double>(nC) / static_cast<double>(result.totalBases) << "%\n";
    os << "#G\t" << nG << "\t"
       << 100.0 * static_cast<double>(nG) / static_cast<double>(result.totalBases) << "%\n";
    os << "#T\t" << nT << "\t"
       << 100.0 * static_cast<double>(nT) / static_cast<double>(result.totalBases) << "%\n";
    os << "#N\t" << nN << "\t"
       << 100.0 * static_cast<double>(nN) / static_cast<double>(result.totalBases) << "%\n";
    os << "#GC\t" << nG + nC << "\t"
       << 100.0 * static_cast<double>(nG + nC) / static_cast<double>(result.totalBases) << "%\n";

    // Per-position statistics
    os << "#Pos\tA\tC\tG\tT\tN\tAvgQual\tErrRate\n";
    for (size_t i = 0; i < result.maxReadLength; ++i) {
        const uint64_t* bSlot = result.baseAt(i);
        const uint64_t* qSlot = result.qualityAt(i);

        os << i + 1 << "\t";
        os << bSlot[0] << "\t" << bSlot[1] << "\t" << bSlot[2] << "\t" << bSlot[3] << "\t"
           << bSlot[4] << "\t";

        uint64_t sumQual = 0;
        uint64_t countReadsAtPos = 0;

        for (int j = 0; j < kMaxQual; ++j) {
            sumQual += qSlot[j] * j;
            countReadsAtPos += qSlot[j];
        }

        if (countReadsAtPos > 0) {
            os << static_cast<double>(sumQual) / static_cast<double>(countReadsAtPos) << "\t";
            os << calculateErrorPerPosition(qSlot, countReadsAtPos) << "\n";
        } else {
            os << "0.0\t0.0\n";
        }
    }
}

void StatisticsWriter::writeSignature(std::ostream& os, const FqStatisticResult& result) {
    os << "metric\tkey\tcount\n";
    os << "summary\ttotal_reads\t" << result.readCount << "\n";
    os << "summary\tduplicate_estimate\t"
       << estimateDuplicates(result.duplicateSampledReads, options_.duplicateEstimateSampleModulo)
       << "\n";

    for (const auto& [kmer, count] :
         sortedTopEntries(result.headKmerCounts, options_.maxReportedSignatures)) {
        os << "head_kmer\t" << kmer << "\t" << count << "\n";
    }
}

}  // namespace fq::statistic
