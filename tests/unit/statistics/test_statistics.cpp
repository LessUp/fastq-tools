#include "statistics/fq_statistic_worker.h"
#include "statistics/fq_statistic.h"
#include "fqtools/io/fastq_io.h"

#include <gtest/gtest.h>
#include <vector>

namespace fq::statistic {

TEST(FqStatisticResultTest, OperatorPlusEquals) {
    FqStatisticResult res1;
    res1.readCount = 10;
    res1.totalBases = 1000;
    res1.maxReadLength = 100;
    res1.posQualityDist.resize(100, std::vector<uint64_t>(kMaxQual, 1));
    res1.posBaseDist.resize(100, std::vector<uint64_t>(kMaxBaseNum, 2));

    FqStatisticResult res2;
    res2.readCount = 5;
    res2.totalBases = 500;
    res2.maxReadLength = 120; // Longer reads
    res2.posQualityDist.resize(120, std::vector<uint64_t>(kMaxQual, 3));
    res2.posBaseDist.resize(120, std::vector<uint64_t>(kMaxBaseNum, 4));

    res1 += res2;

    EXPECT_EQ(res1.readCount, 15);
    EXPECT_EQ(res1.totalBases, 1500);
    EXPECT_EQ(res1.maxReadLength, 120);
    
    // Check combined distributions for first 100 positions
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(res1.posQualityDist[i][0], 4); // 1 + 3
        EXPECT_EQ(res1.posBaseDist[i][0], 6);    // 2 + 4
    }
    
    // Check distribution for positions 100-120 (only from res2)
    for (int i = 100; i < 120; ++i) {
        EXPECT_EQ(res1.posQualityDist[i][0], 3);
        EXPECT_EQ(res1.posBaseDist[i][0], 4);
    }
}

TEST(FqStatisticWorkerTest, CalculateStats) {
    FqStatisticWorker worker(33); // Sanger offset
    fq::io::FastqBatch batch;
    
    fq::io::FastqRecord rec1;
    rec1.id = "read1";
    rec1.seq = "ACGTN"; // length 5
    rec1.qual = "!!#$!"; // Qualities: 0, 0, 2, 3, 0 (offset 33)
    batch.records().push_back(rec1);

    fq::io::FastqRecord rec2;
    rec2.id = "read2";
    rec2.seq = "AAAAA"; // length 5
    rec2.qual = "IIIII"; // Quality: 40 (offset 33)
    batch.records().push_back(rec2);

    auto result = worker.calculateStats(batch);

    EXPECT_EQ(result.readCount, 2);
    EXPECT_EQ(result.totalBases, 10);
    EXPECT_EQ(result.maxReadLength, 5);

    // Check base distribution at pos 0: 1 A (from rec2), 1 A (from rec1) -> 2 A
    EXPECT_EQ(result.posBaseDist[0][0], 2); // A
    
    // Check N count at pos 4: 1 N (from rec1)
    EXPECT_EQ(result.posBaseDist[4][4], 1); // N
    
    // Check Quality distribution at pos 0: 1 '!' (0), 1 'I' (40)
    EXPECT_EQ(result.posQualityDist[0][0], 1);
    EXPECT_EQ(result.posQualityDist[0][40], 1);
}

TEST(FqStatisticWorkerTest, EmptyBatch) {
    FqStatisticWorker worker;
    fq::io::FastqBatch batch;
    auto result = worker.calculateStats(batch);
    EXPECT_EQ(result.readCount, 0);
    EXPECT_TRUE(result.posQualityDist.empty());
}

} // namespace fq::statistic
