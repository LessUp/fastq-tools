#include <algorithm>

#include "statistics/fq_statistic.h"
#include "statistics/statistics_report.h"
#include <fqtools/statistics/statistics_writer.h>
#include <gtest/gtest.h>

namespace fq::statistic {

TEST(StatisticsReportTest, BuildsSummaryPositionAndSignatureLines) {
    FqStatisticResult result;
    result.readCount = 2;
    result.totalBases = 4;
    result.duplicateSampledReads = 1;
    result.ensureCapacity(2);

    result.baseAt(0)[0] = 1;
    result.baseAt(0)[1] = 1;
    result.baseAt(1)[2] = 1;
    result.baseAt(1)[3] = 1;

    result.qualityAt(0)[20] = 2;
    result.qualityAt(1)[30] = 2;

    result.headKmerCounts["ACGT"] = 2;

    StatisticsWriterOptions options;
    options.inputFastqPath = "/tmp/input.fastq";
    options.qualityEncoding = 33;
    options.duplicateEstimateSampleModulo = 2;
    options.maxReportedSignatures = 5;

    const auto report = buildStatisticsReport(result, options);

    EXPECT_EQ(report.summaryLines.front(), "#Name\tinput.fastq");
    EXPECT_NE(
        std::find(report.summaryLines.begin(), report.summaryLines.end(), "#DuplicateEstimate\t2"),
        report.summaryLines.end());
    EXPECT_EQ(report.positionLines.front(), "#Pos\tA\tC\tG\tT\tN\tAvgQual\tErrRate");
    EXPECT_EQ(report.signatureLines.front(), "metric\tkey\tcount");
    EXPECT_NE(
        std::find(report.signatureLines.begin(), report.signatureLines.end(), "head_kmer\tACGT\t2"),
        report.signatureLines.end());
}

TEST(StatisticsReportTest, UsesPerPositionQualityStrideForErrorRate) {
    FqStatisticResult result;
    result.readCount = 201;
    result.totalBases = 201;
    result.ensureCapacity(2);

    result.baseAt(0)[0] = 1;
    result.baseAt(1)[0] = 200;
    result.qualityAt(0)[20] = 1;
    result.qualityAt(1)[0] = 200;

    StatisticsWriterOptions options;
    options.inputFastqPath = "/tmp/input.fastq";
    options.qualityEncoding = 33;
    options.duplicateEstimateSampleModulo = 1;
    options.maxReportedSignatures = 5;

    const auto report = buildStatisticsReport(result, options);

    ASSERT_GE(report.positionLines.size(), 3U);
    EXPECT_EQ(report.positionLines[1], "1\t1\t0\t0\t0\t0\t20.00\t1.00");
}

TEST(StatisticsReportTest, HandlesZeroTotalBasesWithoutInfOrNanSummary) {
    FqStatisticResult result;
    result.readCount = 2;
    result.totalBases = 0;

    StatisticsWriterOptions options;
    options.inputFastqPath = "/tmp/input.fastq";
    options.qualityEncoding = 33;
    options.duplicateEstimateSampleModulo = 2;
    options.maxReportedSignatures = 5;

    const auto report = buildStatisticsReport(result, options);

    EXPECT_NE(
        std::find(report.summaryLines.begin(), report.summaryLines.end(), "#Q20(>=20)\t0\t0.00%"),
        report.summaryLines.end());
    EXPECT_NE(std::find(report.summaryLines.begin(), report.summaryLines.end(), "#GC\t0\t0.00%"),
              report.summaryLines.end());
}

}  // namespace fq::statistic
