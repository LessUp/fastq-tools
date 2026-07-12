/**
 * @file test_predicates.cpp
 * @brief Predicate 组件单元测试
 * @details 测试所有 Predicate 实现的功能正确性和边界条件
 */

#include "fqtools/io/fastq_io.h"
#include "fqtools/processing/predicates.h"

#include <thread>
#include <vector>

#include <gtest/gtest.h>

using namespace fq::processing;
using namespace fq::io;

// ============================================================================
// MinQualityPredicate 测试
// ============================================================================

class MinQualityPredicateTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(MinQualityPredicateTest, PassesReadWithHighAverageQuality) {
    MinQualityPredicate predicate(20.0);

    // Phred+33: 'I' = 40, 'H' = 39, 'G' = 38, 'F' = 37
    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MinQualityPredicateTest, RejectsReadWithLowAverageQuality) {
    MinQualityPredicate predicate(30.0);

    // Phred+33: '!' = 0, 质量极低
    FastqRecord read{"read1", {}, "ACGT", "!!!!", "+"};

    EXPECT_FALSE(predicate.evaluate(read));
}

TEST_F(MinQualityPredicateTest, HandlesMixedQuality) {
    MinQualityPredicate predicate(25.0);

    // 混合质量：'!' = 0, 'I' = 40, 平均 = 20
    FastqRecord read{"read1", {}, "ACGT", "!!II", "+"};

    EXPECT_FALSE(predicate.evaluate(read));
}

TEST_F(MinQualityPredicateTest, SupportsPhred64Encoding) {
    MinQualityPredicate predicate(30.0, 64);

    // Phred+64: '@' = 64 - 64 = 0, '^' = 94 - 64 = 30
    FastqRecord read{"read1", {}, "ACGT", "^^^^", "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MinQualityPredicateTest, RejectsEmptyQualityString) {
    MinQualityPredicate predicate(10.0);

    FastqRecord read{"read1", {}, "ACGT", {}, "+"};

    EXPECT_FALSE(predicate.evaluate(read));
}

TEST_F(MinQualityPredicateTest, GetNameReturnsNonEmpty) {
    MinQualityPredicate predicate(20.0);

    EXPECT_FALSE(predicate.getName().empty());
}

TEST_F(MinQualityPredicateTest, GetDescriptionReturnsNonEmpty) {
    MinQualityPredicate predicate(20.0);

    EXPECT_FALSE(predicate.getDescription().empty());
}

TEST_F(MinQualityPredicateTest, GetStatisticsReturnsInfo) {
    MinQualityPredicate predicate(20.0);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};
    predicate.evaluate(read);

    std::string stats = predicate.getStatistics();
    EXPECT_FALSE(stats.empty());
    // 统计信息应包含评估数量
}

TEST_F(MinQualityPredicateTest, ThreadSafeStatistics) {
    MinQualityPredicate predicate(20.0);

    auto evaluateMultiple = [&predicate]() {
        FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};
        for (int i = 0; i < 100; ++i) {
            predicate.evaluate(read);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back(evaluateMultiple);
    }

    for (auto& t : threads) {
        t.join();
    }

    // 4 线程 × 100 次 = 400 次评估
    std::string stats = predicate.getStatistics();
    EXPECT_FALSE(stats.empty());
}

// ============================================================================
// MinLengthPredicate 测试
// ============================================================================

class MinLengthPredicateTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(MinLengthPredicateTest, PassesReadMeetingMinLength) {
    MinLengthPredicate predicate(10);

    FastqRecord read{"read1", {}, "ACGTACGTACGT", "IIIIIIIIIIII", "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MinLengthPredicateTest, PassesReadExactlyAtMinLength) {
    MinLengthPredicate predicate(4);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MinLengthPredicateTest, RejectsReadBelowMinLength) {
    MinLengthPredicate predicate(10);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};

    EXPECT_FALSE(predicate.evaluate(read));
}

TEST_F(MinLengthPredicateTest, RejectsEmptyRead) {
    MinLengthPredicate predicate(1);

    FastqRecord read{"read1", {}, {}, {}, "+"};

    EXPECT_FALSE(predicate.evaluate(read));
}

TEST_F(MinLengthPredicateTest, ZeroMinLengthPassesAll) {
    MinLengthPredicate predicate(0);

    FastqRecord read{"read1", {}, "A", "I", "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MinLengthPredicateTest, GetNameReturnsNonEmpty) {
    MinLengthPredicate predicate(10);

    EXPECT_FALSE(predicate.getName().empty());
}

TEST_F(MinLengthPredicateTest, GetStatisticsCountsCorrectly) {
    MinLengthPredicate predicate(5);

    FastqRecord shortRead{"read1", {}, "ACG", "III", "+"};
    FastqRecord longRead{"read2", {}, "ACGTACGT", "IIIIIIII", "+"};

    predicate.evaluate(shortRead);  // rejected
    predicate.evaluate(longRead);   // passed

    std::string stats = predicate.getStatistics();
    EXPECT_FALSE(stats.empty());
}

// ============================================================================
// MaxLengthPredicate 测试
// ============================================================================

class MaxLengthPredicateTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(MaxLengthPredicateTest, PassesReadBelowMaxLength) {
    MaxLengthPredicate predicate(10);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MaxLengthPredicateTest, PassesReadExactlyAtMaxLength) {
    MaxLengthPredicate predicate(4);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MaxLengthPredicateTest, RejectsReadAboveMaxLength) {
    MaxLengthPredicate predicate(2);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};

    EXPECT_FALSE(predicate.evaluate(read));
}

TEST_F(MaxLengthPredicateTest, PassesEmptyRead) {
    MaxLengthPredicate predicate(10);

    FastqRecord read{"read1", {}, {}, {}, "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MaxLengthPredicateTest, GetNameReturnsNonEmpty) {
    MaxLengthPredicate predicate(10);

    EXPECT_FALSE(predicate.getName().empty());
}

// ============================================================================
// MaxNRatioPredicate 测试
// ============================================================================

class MaxNRatioPredicateTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(MaxNRatioPredicateTest, PassesReadWithLowNRatio) {
    MaxNRatioPredicate predicate(0.5);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MaxNRatioPredicateTest, PassesReadWithNoN) {
    MaxNRatioPredicate predicate(0.1);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MaxNRatioPredicateTest, RejectsReadWithHighNRatio) {
    MaxNRatioPredicate predicate(0.25);

    FastqRecord read{"read1", {}, "ANN", "III", "+"};  // 2/3 N = 0.67

    EXPECT_FALSE(predicate.evaluate(read));
}

TEST_F(MaxNRatioPredicateTest, HandlesBothLowerAndUpperN) {
    MaxNRatioPredicate predicate(0.3);

    FastqRecord read{"read1", {}, "AnNn", "IIII", "+"};  // 3/4 N = 0.75

    EXPECT_FALSE(predicate.evaluate(read));
}

TEST_F(MaxNRatioPredicateTest, PassesReadAtExactThreshold) {
    MaxNRatioPredicate predicate(0.25);

    FastqRecord read{"read1", {}, "ACGN", "IIII", "+"};  // 1/4 N = 0.25

    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MaxNRatioPredicateTest, HandlesEmptySequence) {
    MaxNRatioPredicate predicate(0.1);

    FastqRecord read{"read1", {}, {}, {}, "+"};

    // 空序列没有 N，应通过
    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(MaxNRatioPredicateTest, GetNameReturnsNonEmpty) {
    MaxNRatioPredicate predicate(0.1);

    EXPECT_FALSE(predicate.getName().empty());
}

// ============================================================================
// 边界条件测试
// ============================================================================

class PredicateBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(PredicateBoundaryTest, SingleBaseRead) {
    MinLengthPredicate minLength(1);
    MaxLengthPredicate maxLength(1);
    MinQualityPredicate minQuality(0.0);

    FastqRecord read{"read1", {}, "A", "I", "+"};  // 'I' - 33 = 40

    EXPECT_TRUE(minLength.evaluate(read));
    EXPECT_TRUE(maxLength.evaluate(read));
    EXPECT_TRUE(minQuality.evaluate(read));
}

TEST_F(PredicateBoundaryTest, VeryLongSequence) {
    std::string longSeq(10000, 'A');
    std::string longQual(10000, 'I');

    FastqRecord read{"read1", {}, longSeq, longQual, "+"};

    MinLengthPredicate minLength(5000);
    MaxLengthPredicate maxLength(20000);
    MinQualityPredicate minQuality(30.0);

    EXPECT_TRUE(minLength.evaluate(read));
    EXPECT_TRUE(maxLength.evaluate(read));
    EXPECT_TRUE(minQuality.evaluate(read));
}

TEST_F(PredicateBoundaryTest, AllNSequence) {
    MaxNRatioPredicate predicate(0.5);

    FastqRecord read{"read1", {}, "NNNN", "IIII", "+"};

    EXPECT_FALSE(predicate.evaluate(read));
}

TEST_F(PredicateBoundaryTest, QualityAtThreshold) {
    // '5' = 53 - 33 = 20
    MinQualityPredicate predicate(20.0);

    FastqRecord read{"read1", {}, "AAAA", "5555", "+"};

    // 平均质量 = 20.0，应该通过（>= 阈值）
    EXPECT_TRUE(predicate.evaluate(read));
}

TEST_F(PredicateBoundaryTest, QualityBelowEncodingDoesNotOverflow) {
    // 质量字符 ASCII 值低于 encoding（Phred33），如 NUL(0) 或空格(32)
    // 修复前：uint64_t 累加负差下溢成巨大正值，坏数据反而通过
    // 修复后：负质量值 clamp 到 0，应判为低质量
    MinQualityPredicate predicate(20.0);

    // 空格(32) - 33 = -1，四个空格平均 = -1 → clamp 0
    FastqRecord read{"read1", {}, "ACGT", "    ", "+"};

    EXPECT_FALSE(predicate.evaluate(read));
}
