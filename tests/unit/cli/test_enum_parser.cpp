/**
 * @file test_enum_parser.cpp
 * @brief enum_parser 单元测试
 * @details 覆盖 parseProcessingProfile 和 parseTrimMode 的合法值、边界值、非法值
 */

#include "enum_parser.h"

#include <gtest/gtest.h>

using namespace fq::cli;
using namespace fq::processing;

// ============================================================================
// parseProcessingProfile 测试
// ============================================================================

TEST(EnumParserTest, ParseDefaultProfile) {
    EXPECT_EQ(parseProcessingProfile("default"), ProcessingProfile::Default);
}

TEST(EnumParserTest, ParseLowMemoryProfile) {
    EXPECT_EQ(parseProcessingProfile("lowMemory"), ProcessingProfile::LowMemory);
}

TEST(EnumParserTest, ParseHighThroughputProfile) {
    EXPECT_EQ(parseProcessingProfile("highThroughput"), ProcessingProfile::HighThroughput);
}

TEST(EnumParserTest, ParseInvalidProfileThrows) {
    EXPECT_THROW(static_cast<void>(parseProcessingProfile("invalid")), std::invalid_argument);
}

TEST(EnumParserTest, ParseEmptyProfileThrows) {
    EXPECT_THROW(static_cast<void>(parseProcessingProfile("")), std::invalid_argument);
}

TEST(EnumParserTest, ParseProfileCaseSensitive) {
    // 大小写敏感：Default 应抛异常
    EXPECT_THROW(static_cast<void>(parseProcessingProfile("Default")), std::invalid_argument);
}

// ============================================================================
// parseTrimMode 测试
// ============================================================================

TEST(EnumParserTest, ParseTrimModeFive) {
    EXPECT_EQ(parseTrimMode("five"), QualityTrimmer::TrimMode::FivePrime);
}

TEST(EnumParserTest, ParseTrimModeThree) {
    EXPECT_EQ(parseTrimMode("three"), QualityTrimmer::TrimMode::ThreePrime);
}

TEST(EnumParserTest, ParseTrimModeBoth) {
    EXPECT_EQ(parseTrimMode("both"), QualityTrimmer::TrimMode::Both);
}

TEST(EnumParserTest, ParseInvalidTrimModeThrows) {
    EXPECT_THROW(static_cast<void>(parseTrimMode("invalid")), std::invalid_argument);
}

TEST(EnumParserTest, ParseEmptyTrimModeThrows) {
    EXPECT_THROW(static_cast<void>(parseTrimMode("")), std::invalid_argument);
}
