#include "fqtools/io/fastq_io.h"

#include <vector>

#include "statistics/fq_statistic.h"
#include "statistics/fq_statistic_worker.h"
#include <gtest/gtest.h>

namespace fq::statistics {

TEST(FqStatisticResultTest, OperatorPlusEquals) {
    FqStatisticResult res1;
    res1.readCount = 10;
    res1.totalBases = 1000;
    res1.maxReadLength = 100;
    res1.posQualityDist.resize(100 * kMaxQual, 1);
    res1.posBaseDist.resize(100 * kMaxBaseNum, 2);

    FqStatisticResult res2;
    res2.readCount = 5;
    res2.totalBases = 500;
    res2.maxReadLength = 120;  // Longer reads
    res2.posQualityDist.resize(120 * kMaxQual, 3);
    res2.posBaseDist.resize(120 * kMaxBaseNum, 4);

    res1 += res2;

    EXPECT_EQ(res1.readCount, 15);
    EXPECT_EQ(res1.totalBases, 1500);
    EXPECT_EQ(res1.maxReadLength, 120);

    // Check combined distributions for first 100 positions (flat layout: pos * stride + slot)
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(res1.qualityAt(i)[0], 4);  // 1 + 3
        EXPECT_EQ(res1.baseAt(i)[0], 6);     // 2 + 4
    }

    // Check distribution for positions 100-120 (only from res2)
    for (int i = 100; i < 120; ++i) {
        EXPECT_EQ(res1.qualityAt(i)[0], 3);
        EXPECT_EQ(res1.baseAt(i)[0], 4);
    }
}

TEST(FqStatisticResultTest, AggregatesDuplicateAndSignatureMaps) {
    FqStatisticResult left;
    left.duplicateSampledReads = 1;
    left.headKmerCounts["AAAA"] = 2;
    left.sampledSequenceHashes[1] = 2;

    FqStatisticResult right;
    right.duplicateSampledReads = 4;
    right.headKmerCounts["AAAA"] = 4;
    right.headKmerCounts["TTTT"] = 1;
    right.sampledSequenceHashes[1] = 5;

    left += right;

    EXPECT_EQ(left.duplicateSampledReads, 6);
    EXPECT_EQ(left.headKmerCounts["AAAA"], 6);
    EXPECT_EQ(left.headKmerCounts["TTTT"], 1);
    EXPECT_EQ(left.sampledSequenceHashes[1], 7);
}

TEST(FqStatisticWorkerTest, CalculateStats) {
    FqStatisticWorker worker(33);  // Sanger offset
    fq::io::FastqBatch batch;

    fq::io::FastqRecord rec1;
    rec1.id = "read1";
    rec1.seq = "ACGTN";   // length 5
    rec1.qual = "!!#$!";  // Qualities: 0, 0, 2, 3, 0 (offset 33)
    batch.records().push_back(rec1);

    fq::io::FastqRecord rec2;
    rec2.id = "read2";
    rec2.seq = "AAAAA";   // length 5
    rec2.qual = "IIIII";  // Quality: 40 (offset 33)
    batch.records().push_back(rec2);

    auto result = worker.calculateStats(batch);

    EXPECT_EQ(result.readCount, 2);
    EXPECT_EQ(result.totalBases, 10);
    EXPECT_EQ(result.maxReadLength, 5);

    // Check base distribution at pos 0: 1 A (from rec2), 1 A (from rec1) -> 2 A
    EXPECT_EQ(result.baseAt(0)[0], 2);  // A

    // Check N count at pos 4: 1 N (from rec1)
    EXPECT_EQ(result.baseAt(4)[4], 1);  // N

    // Check Quality distribution at pos 0: 1 '!' (0), 1 'I' (40)
    EXPECT_EQ(result.qualityAt(0)[0], 1);
    EXPECT_EQ(result.qualityAt(0)[40], 1);
}

TEST(FqStatisticWorkerTest, RespectsConfiguredQualityEncoding) {
    fq::io::FastqBatch batch;

    fq::io::FastqRecord rec;
    rec.id = "read1";
    rec.seq = "A";
    rec.qual = "@";
    batch.records().push_back(rec);

    FqStatisticWorker worker33(33);
    auto result33 = worker33.calculateStats(batch);
    EXPECT_EQ(result33.qualityAt(0)[31], 1);

    FqStatisticWorker worker64(64);
    auto result64 = worker64.calculateStats(batch);
    EXPECT_EQ(result64.qualityAt(0)[0], 1);
}

TEST(FqStatisticWorkerTest, EmptyBatch) {
    FqStatisticWorker worker;
    fq::io::FastqBatch batch;
    auto result = worker.calculateStats(batch);
    EXPECT_EQ(result.readCount, 0);
    EXPECT_TRUE(result.posQualityDist.empty());
}

TEST(FqStatisticWorkerTest, TracksDuplicatesAndHeadKmers) {
    FqStatisticWorker worker(33, 4, 1);
    fq::io::FastqBatch batch;

    fq::io::FastqRecord rec1{"read1", {}, "ACGTAAAA", "IIIIIIII", "+"};
    fq::io::FastqRecord rec2{"read2", {}, "ACGTAAAA", "IIIIIIII", "+"};
    fq::io::FastqRecord rec3{"read3", {}, "TTTTCCCC", "IIIIIIII", "+"};

    batch.records().push_back(rec1);
    batch.records().push_back(rec2);
    batch.records().push_back(rec3);

    const auto result = worker.calculateStats(batch);

    EXPECT_EQ(result.readCount, 3);
    EXPECT_EQ(result.duplicateSampledReads, 1);
    EXPECT_EQ(result.headKmerCounts.at("ACGT"), 2);
    EXPECT_EQ(result.headKmerCounts.at("TTTT"), 1);
}

}  // namespace fq::statistics
