/**
 * @file test_mutators.cpp
 * @brief Mutator 组件单元测试
 * @details 测试所有 Mutator 实现的功能正确性和边界条件
 */

#include "fqtools/io/fastq_io.h"
#include "fqtools/processing/mutators.h"

#include <gtest/gtest.h>

using namespace fq::processing;
using namespace fq::io;

// ============================================================================
// QualityTrimmer 测试
// ============================================================================

class QualityTrimmerTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(QualityTrimmerTest, TrimsLowQualityFromBothEnds) {
    QualityTrimmer trimmer(20.0);

    // '!' = 0, 'I' = 40
    FastqRecord read{"read1", {}, "ACGTACGT", "!!IIII!!", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "GTAC");
    EXPECT_EQ(read.qual, "IIII");
}

TEST_F(QualityTrimmerTest, TrimsOnlyFivePrime) {
    QualityTrimmer trimmer(20.0, 1, QualityTrimmer::TrimMode::FivePrime);

    FastqRecord read{"read1", {}, "ACGT", "!!II", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "GT");
    EXPECT_EQ(read.qual, "II");
}

TEST_F(QualityTrimmerTest, TrimsOnlyThreePrime) {
    QualityTrimmer trimmer(20.0, 1, QualityTrimmer::TrimMode::ThreePrime);

    FastqRecord read{"read1", {}, "ACGT", "II!!", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "AC");
    EXPECT_EQ(read.qual, "II");
}

TEST_F(QualityTrimmerTest, DropsReadShorterThanMinLength) {
    QualityTrimmer trimmer(20.0, 5);  // 最小长度 5

    FastqRecord read{"read1", {}, "ACGT", "!!II", "+"};
    trimmer.process(read);

    EXPECT_TRUE(read.seq.empty());
    EXPECT_TRUE(read.qual.empty());
}

TEST_F(QualityTrimmerTest, KeepsReadMeetingMinLength) {
    QualityTrimmer trimmer(20.0, 2);  // 最小长度 2

    FastqRecord read{"read1", {}, "ACGT", "!!II", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq.size(), 2);
}

TEST_F(QualityTrimmerTest, SupportsPhred64Encoding) {
    QualityTrimmer trimmer(20.0, 1, QualityTrimmer::TrimMode::Both, 64);

    // Phred+64: '@' = 0, '^' = 30
    FastqRecord read{"read1", {}, "ACGT", "@^^^", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "CGT");
    EXPECT_EQ(read.qual, "^^^");
}

TEST_F(QualityTrimmerTest, NoTrimWhenAllHighQuality) {
    QualityTrimmer trimmer(20.0);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGT");
    EXPECT_EQ(read.qual, "IIII");
}

TEST_F(QualityTrimmerTest, DropsAllWhenAllLowQuality) {
    QualityTrimmer trimmer(20.0);

    FastqRecord read{"read1", {}, "ACGT", "!!!!", "+"};
    trimmer.process(read);

    EXPECT_TRUE(read.seq.empty());
    EXPECT_TRUE(read.qual.empty());
}

TEST_F(QualityTrimmerTest, HandlesEmptyRead) {
    QualityTrimmer trimmer(20.0);

    FastqRecord read{"read1", {}, {}, {}, "+"};
    trimmer.process(read);

    EXPECT_TRUE(read.seq.empty());
}

TEST_F(QualityTrimmerTest, GetNameReturnsNonEmpty) {
    QualityTrimmer trimmer(20.0);

    EXPECT_FALSE(trimmer.getName().empty());
}

TEST_F(QualityTrimmerTest, GetDescriptionReturnsNonEmpty) {
    QualityTrimmer trimmer(20.0);

    EXPECT_FALSE(trimmer.getDescription().empty());
}

// 回归：小数阈值在 SIMD（AVX2）与标量构建下必须语义一致。
// q >= 20.5 对整数 q 等价于 q >= 21（ceil 语义）；历史 AVX2 路径按 floor
// 截断阈值，会保留 q=20 的碱基，而标量构建剪掉它。
TEST_F(QualityTrimmerTest, FractionalThresholdUsesCeilSemantics) {
    QualityTrimmer trimmer(20.5, 1, QualityTrimmer::TrimMode::FivePrime);

    // Phred+33: '5' = q20, '6' = q21
    FastqRecord read{"read1", {}, "ACGTACGT", "55556666", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGT");
    EXPECT_EQ(read.qual, "6666");
}

// 长 qual 串（>=32 字符）覆盖 SIMD 循环路径，小数阈值行为与标量一致
TEST_F(QualityTrimmerTest, FractionalThresholdConsistentOnSimdLengthInput) {
    QualityTrimmer trimmer(20.5, 1, QualityTrimmer::TrimMode::FivePrime);

    // FastqRecord 字段为 string_view：必须用命名存储，不能传临时 string
    const std::string low(36, '5');  // q20 × 36
    const std::string high(4, '6');  // q21 × 4
    const std::string seq(40, 'A');
    const std::string qual = low + high;
    FastqRecord read{"read1", {}, seq, qual, "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq.size(), 4U);
    EXPECT_EQ(read.qual, "6666");
}

// 整数阈值不受 SIMD 路径影响（长串覆盖循环）
TEST_F(QualityTrimmerTest, IntegerThresholdUnchangedOnSimdLengthInput) {
    QualityTrimmer trimmer(20.0, 1, QualityTrimmer::TrimMode::Both);

    const std::string low(33, '5');  // q20 × 33：整数阈值 20 下通过
    const std::string tail(7, '!');  // q0 × 7
    const std::string seq(40, 'A');
    const std::string qual = low + tail;
    FastqRecord read{"read1", {}, seq, qual, "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq.size(), 33U);
}

// 回归：Phred+64 下极端阈值（encoding + threshold > 127）必须全剪；
// 历史 AVX2 路径 set1_epi8 符号回绕，反而完全不剪
TEST_F(QualityTrimmerTest, Phred64ExtremeThresholdTrimsAll) {
    QualityTrimmer trimmer(70.0, 1, QualityTrimmer::TrimMode::FivePrime, 64);

    // '~' = 126 = Phred+64 下 q62，全部低于阈值；长度 >= 32 覆盖 SIMD 路径
    const std::string seq(40, 'A');
    const std::string qual(40, '~');
    FastqRecord read{"read1", {}, seq, qual, "+"};
    trimmer.process(read);

    EXPECT_TRUE(read.seq.empty());
    EXPECT_TRUE(read.qual.empty());
}

// ============================================================================
// LengthTrimmer 测试
// ============================================================================

class LengthTrimmerTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(LengthTrimmerTest, MaxLengthTrimsToExactLength) {
    LengthTrimmer trimmer(3, LengthTrimmer::TrimStrategy::MaxLength);

    FastqRecord read{"read1", {}, "ACGTACGT", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq.size(), 3);
    EXPECT_EQ(read.qual.size(), 3);
}

TEST_F(LengthTrimmerTest, FromStartKeepsSuffix) {
    LengthTrimmer trimmer(3, LengthTrimmer::TrimStrategy::FromStart);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "CGT");
    EXPECT_EQ(read.qual, "III");
}

TEST_F(LengthTrimmerTest, MaxLengthDoesNotExtend) {
    LengthTrimmer trimmer(10, LengthTrimmer::TrimStrategy::MaxLength);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};
    trimmer.process(read);

    // 不会扩展，保持原长度
    EXPECT_EQ(read.seq, "ACGT");
    EXPECT_EQ(read.qual, "IIII");
}

TEST_F(LengthTrimmerTest, MaxLengthTrimsIfExceeded) {
    LengthTrimmer trimmer(2, LengthTrimmer::TrimStrategy::MaxLength);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq.size(), 2);
}

TEST_F(LengthTrimmerTest, HandlesShorterRead) {
    LengthTrimmer trimmer(10, LengthTrimmer::TrimStrategy::MaxLength);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};
    trimmer.process(read);

    // 读取长度 4，目标 10，应保持原长度
    EXPECT_EQ(read.seq.size(), 4);
}

TEST_F(LengthTrimmerTest, HandlesEmptyRead) {
    LengthTrimmer trimmer(3, LengthTrimmer::TrimStrategy::MaxLength);

    FastqRecord read{"read1", {}, {}, {}, "+"};
    trimmer.process(read);

    EXPECT_TRUE(read.seq.empty());
}

TEST_F(LengthTrimmerTest, GetNameReturnsNonEmpty) {
    LengthTrimmer trimmer(3);

    EXPECT_FALSE(trimmer.getName().empty());
}

// ============================================================================
// AdapterTrimmer 测试
// ============================================================================

class AdapterTrimmerTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(AdapterTrimmerTest, RemovesAdapterFromEnd) {
    AdapterTrimmer trimmer({"TTAA"}, 3, 0);

    FastqRecord read{"read1", {}, "ACGTTTAA", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGT");
    EXPECT_EQ(read.qual, "IIII");
}

TEST_F(AdapterTrimmerTest, LeavesReadUntouchedWhenNoAdapter) {
    AdapterTrimmer trimmer({"TTAA"}, 3, 0);

    FastqRecord read{"read1", {}, "ACGTACGT", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGTACGT");
    EXPECT_EQ(read.qual, "IIIIIIII");
}

TEST_F(AdapterTrimmerTest, RespectsMinOverlap) {
    AdapterTrimmer trimmer({"TTAA"}, 4, 0);  // 最小重叠 4

    // 只有 2 个碱基匹配，不满足最小重叠
    FastqRecord read{"read1", {}, "ACGTTA", "IIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGTTA");
}

TEST_F(AdapterTrimmerTest, AllowsMismatches) {
    AdapterTrimmer trimmer({"TTAA"}, 3, 1);  // 允许 1 个错配

    // TTAA vs TTGA: 1 个错配
    FastqRecord read{"read1", {}, "ACGTTTGA", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGT");
}

TEST_F(AdapterTrimmerTest, HandlesMultipleAdapters) {
    AdapterTrimmer trimmer({"TTAA", "GGCC"}, 3, 0);

    FastqRecord read1{"read1", {}, "ACGTTTAA", "IIIIIIII", "+"};
    trimmer.process(read1);
    EXPECT_EQ(read1.seq, "ACGT");

    FastqRecord read2{"read2", {}, "ACGTGGCC", "IIIIIIII", "+"};
    trimmer.process(read2);
    EXPECT_EQ(read2.seq, "ACGT");
}

TEST_F(AdapterTrimmerTest, HandlesEmptyRead) {
    AdapterTrimmer trimmer({"TTAA"}, 3, 0);

    FastqRecord read{"read1", {}, {}, {}, "+"};
    trimmer.process(read);

    EXPECT_TRUE(read.seq.empty());
}

TEST_F(AdapterTrimmerTest, HandlesAdapterLongerThanRead) {
    AdapterTrimmer trimmer({"TTAATTAA"}, 3, 0);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGT");
}

TEST_F(AdapterTrimmerTest, HandlesMinOverlapLongerThanReadWithoutHanging) {
    AdapterTrimmer trimmer({"TTAA"}, 5, 0);

    FastqRecord read{"read1", {}, "ACGT", "IIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGT");
    EXPECT_EQ(read.qual, "IIII");
}

TEST_F(AdapterTrimmerTest, GetNameReturnsNonEmpty) {
    AdapterTrimmer trimmer({"TTAA"}, 3, 0);

    EXPECT_FALSE(trimmer.getName().empty());
}

class PolyTailTrimmerTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(PolyTailTrimmerTest, TrimsPolyGTailWhenRunLengthMet) {
    PolyTailTrimmer trimmer(PolyTailTrimmer::TailKind::PolyG, 4);

    FastqRecord read{"read1", {}, "ACGTGGGG", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGT");
    EXPECT_EQ(read.qual, "IIII");
}

TEST_F(PolyTailTrimmerTest, LeavesReadUntouchedWhenPolyGRunTooShort) {
    PolyTailTrimmer trimmer(PolyTailTrimmer::TailKind::PolyG, 5);

    FastqRecord read{"read1", {}, "ACGTGGGG", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGTGGGG");
    EXPECT_EQ(read.qual, "IIIIIIII");
}

TEST_F(PolyTailTrimmerTest, TrimsPolyXTailWhenTailIsLowComplexity) {
    PolyTailTrimmer trimmer(PolyTailTrimmer::TailKind::PolyX, 4);

    FastqRecord read{"read1", {}, "ACGTTTTT", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACG");
    EXPECT_EQ(read.qual, "III");
}

TEST_F(PolyTailTrimmerTest, DoesNotTrimMixedTailInPolyXMode) {
    PolyTailTrimmer trimmer(PolyTailTrimmer::TailKind::PolyX, 4);

    FastqRecord read{"read1", {}, "ACGTTTTA", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGTTTTA");
    EXPECT_EQ(read.qual, "IIIIIIII");
}

// ============================================================================
// 边界条件测试
// ============================================================================

class MutatorBoundaryTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(MutatorBoundaryTest, SingleBaseRead) {
    QualityTrimmer qualityTrimmer(20.0);
    LengthTrimmer lengthTrimmer(1, LengthTrimmer::TrimStrategy::MaxLength);

    FastqRecord read1{"read1", {}, "A", "I", "+"};
    qualityTrimmer.process(read1);
    EXPECT_EQ(read1.seq, "A");

    FastqRecord read2{"read2", {}, "A", "I", "+"};
    lengthTrimmer.process(read2);
    EXPECT_EQ(read2.seq, "A");
}

TEST_F(MutatorBoundaryTest, VeryLongSequence) {
    std::string longSeq(10000, 'A');
    std::string longQual(10000, 'I');

    FastqRecord read{"read1", {}, longSeq, longQual, "+"};

    QualityTrimmer trimmer(20.0);
    trimmer.process(read);

    // 高质量序列应保持原长度
    EXPECT_EQ(read.seq.size(), 10000);
}

TEST_F(MutatorBoundaryTest, QualityThresholdAtExactValue) {
    // '5' = 53 - 33 = 20
    QualityTrimmer trimmer(20.0);

    FastqRecord read{"read1", {}, "AAAA", "5555", "+"};
    trimmer.process(read);

    // 质量等于阈值，应保留
    EXPECT_EQ(read.seq, "AAAA");
}

TEST_F(MutatorBoundaryTest, MixedQualityPattern) {
    QualityTrimmer trimmer(20.0);

    // 复杂质量模式（低高低…）：5' 端 '!' 被剪，3' 端 'I' 保留
    FastqRecord read{"read1", {}, "ACGTACGT", "!II!II!I", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "CGTACGT");
    EXPECT_EQ(read.qual, "II!II!I");
}

TEST_F(MutatorBoundaryTest, AdapterAtStart) {
    AdapterTrimmer trimmer({"ACGT"}, 4, 0);

    // 接头在开头：find() 命中位置 0，从该位置起全部剪除 → 空 read
    FastqRecord read{"read1", {}, "ACGTTTAA", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_TRUE(read.seq.empty());
    EXPECT_TRUE(read.qual.empty());
}

TEST_F(MutatorBoundaryTest, MultipleAdaptersWithPriority) {
    // 多个接头，应匹配第一个找到的
    AdapterTrimmer trimmer({"TTAA", "TTAA"}, 3, 0);

    FastqRecord read{"read1", {}, "ACGTTTAA", "IIIIIIII", "+"};
    trimmer.process(read);

    EXPECT_EQ(read.seq, "ACGT");
}

// ============================================================================
// 组合测试
// ============================================================================

class MutatorCompositionTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(MutatorCompositionTest, QualityThenLengthTrimmer) {
    QualityTrimmer qualityTrimmer(20.0);
    LengthTrimmer lengthTrimmer(2, LengthTrimmer::TrimStrategy::MaxLength);

    FastqRecord read{"read1", {}, "ACGTACGT", "!!IIII!!", "+"};

    // 先质量修剪
    qualityTrimmer.process(read);
    EXPECT_EQ(read.seq, "GTAC");

    // 再长度修剪
    lengthTrimmer.process(read);
    EXPECT_EQ(read.seq, "GT");
}

TEST_F(MutatorCompositionTest, AdapterThenQualityTrimmer) {
    AdapterTrimmer adapterTrimmer({"TTAA"}, 3, 0);
    QualityTrimmer qualityTrimmer(20.0);

    // 接头在末尾，质量中间低两边高
    FastqRecord read{"read1", {}, "ACGTTTAA", "IIIIIIII", "+"};

    // 先接头去除
    adapterTrimmer.process(read);
    EXPECT_EQ(read.seq, "ACGT");

    // 接头已去除，序列全是高质量
    qualityTrimmer.process(read);
    EXPECT_EQ(read.seq, "ACGT");
}
