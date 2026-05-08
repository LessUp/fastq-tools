#include "statistics/statistics_report.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <map>
#include <sstream>
#include <vector>

#include "statistics/fq_statistic.h"

namespace fq::statistic {

namespace {

constexpr int kWriterMaxQual = 128;
constexpr int kQ20Threshold = 20;
constexpr int kQ30Threshold = 30;

auto calculateErrorPerPosition(const uint64_t* qualSlot, uint64_t totalBases) -> double {
    if (totalBases == 0) {
        return 0.0;
    }

    double sumProb = 0.0;
    for (int q = 0; q < kWriterMaxQual; ++q) {
        sumProb +=
            static_cast<double>(qualSlot[q]) * std::pow(10.0, -static_cast<double>(q) / 10.0);
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

auto formatMetricLine(const std::string& name, uint64_t count, uint64_t totalBases) -> std::string {
    std::ostringstream line;
    line << std::fixed << std::setprecision(2);
    line << name << '\t' << count << '\t'
         << 100.0 * static_cast<double>(count) / static_cast<double>(totalBases) << '%';
    return line.str();
}

}  // namespace

auto buildStatisticsReport(const FqStatisticResult& result,
                           const StatisticsWriterOptions& options) -> StatisticsReport {
    StatisticsReport report;
    if (result.readCount == 0) {
        return report;
    }

    const std::string fqName = std::filesystem::path(options.inputFastqPath).filename().string();
    const auto duplicateEstimate =
        estimateDuplicates(result.duplicateSampledReads, options.duplicateEstimateSampleModulo);

    report.summaryLines.push_back("#Name\t" + fqName);
    report.summaryLines.push_back("#PhredQual\t" + std::to_string(options.qualityEncoding));
    report.summaryLines.push_back("#ReadNum\t" + std::to_string(result.readCount));
    report.summaryLines.push_back("#DuplicateEstimate\t" + std::to_string(duplicateEstimate));
    {
        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << "#DuplicateEstimateRate\t"
             << 100.0 * static_cast<double>(duplicateEstimate) /
                static_cast<double>(result.readCount)
             << '%';
        report.summaryLines.push_back(line.str());
    }
    report.summaryLines.push_back("#MaxReadLength\t" + std::to_string(result.maxReadLength));
    report.summaryLines.push_back("#BaseCount\t" + std::to_string(result.totalBases));

    uint64_t nQ20 = 0;
    uint64_t nQ30 = 0;
    uint64_t nA = 0;
    uint64_t nC = 0;
    uint64_t nG = 0;
    uint64_t nT = 0;
    uint64_t nN = 0;

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

    report.summaryLines.push_back(formatMetricLine("#Q20(>=20)", nQ20, result.totalBases));
    report.summaryLines.push_back(formatMetricLine("#Q30(>=30)", nQ30, result.totalBases));
    report.summaryLines.push_back(formatMetricLine("#A", nA, result.totalBases));
    report.summaryLines.push_back(formatMetricLine("#C", nC, result.totalBases));
    report.summaryLines.push_back(formatMetricLine("#G", nG, result.totalBases));
    report.summaryLines.push_back(formatMetricLine("#T", nT, result.totalBases));
    report.summaryLines.push_back(formatMetricLine("#N", nN, result.totalBases));
    report.summaryLines.push_back(formatMetricLine("#GC", nG + nC, result.totalBases));

    report.positionLines.push_back("#Pos\tA\tC\tG\tT\tN\tAvgQual\tErrRate");
    for (size_t i = 0; i < result.maxReadLength; ++i) {
        const uint64_t* bSlot = result.baseAt(i);
        const uint64_t* qSlot = result.qualityAt(i);

        uint64_t sumQual = 0;
        uint64_t countReadsAtPos = 0;
        for (int j = 0; j < kMaxQual; ++j) {
            sumQual += qSlot[j] * j;
            countReadsAtPos += qSlot[j];
        }

        std::ostringstream line;
        line << std::fixed << std::setprecision(2);
        line << i + 1 << '\t' << bSlot[0] << '\t' << bSlot[1] << '\t' << bSlot[2] << '\t'
             << bSlot[3] << '\t' << bSlot[4] << '\t';

        if (countReadsAtPos > 0) {
            line << static_cast<double>(sumQual) / static_cast<double>(countReadsAtPos) << '\t'
                 << calculateErrorPerPosition(qSlot, countReadsAtPos);
        } else {
            line << "0.0\t0.0";
        }

        report.positionLines.push_back(line.str());
    }

    report.signatureLines.push_back("metric\tkey\tcount");
    report.signatureLines.push_back("summary\ttotal_reads\t" + std::to_string(result.readCount));
    report.signatureLines.push_back("summary\tduplicate_estimate\t" +
                                    std::to_string(duplicateEstimate));
    for (const auto& [kmer, count] :
         sortedTopEntries(result.headKmerCounts, options.maxReportedSignatures)) {
        report.signatureLines.push_back("head_kmer\t" + kmer + '\t' + std::to_string(count));
    }

    return report;
}

}  // namespace fq::statistic
