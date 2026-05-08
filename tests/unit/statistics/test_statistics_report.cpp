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

}  // namespace fq::statistic
