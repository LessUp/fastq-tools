/**
 * @file enum_parser.cpp
 * @brief 统一的字符串-枚举转换实现
 *
 * @author LessUp
 * @date 2026-05-08
 * @version 2.0
 * @copyright (c) 2026 LessUp. All rights reserved.
 */

#include "enum_parser.h"

#include <stdexcept>

namespace fq::cli {

auto parseProcessingProfile(const std::string& value) -> processing::ProcessingProfile {
    if (value == "default") {
        return processing::ProcessingProfile::Default;
    }
    if (value == "lowMemory") {
        return processing::ProcessingProfile::LowMemory;
    }
    if (value == "highThroughput") {
        return processing::ProcessingProfile::HighThroughput;
    }

    throw std::invalid_argument("Invalid profile: " + value +
                                ". Valid options: default, lowMemory, highThroughput");
}

auto parseTrimMode(const std::string& value) -> processing::QualityTrimmer::TrimMode {
    if (value == "five") {
        return processing::QualityTrimmer::TrimMode::FivePrime;
    }
    if (value == "three") {
        return processing::QualityTrimmer::TrimMode::ThreePrime;
    }
    if (value == "both") {
        return processing::QualityTrimmer::TrimMode::Both;
    }

    throw std::invalid_argument("Invalid trim mode: " + value +
                                ". Valid options: five, three, both");
}

}  // namespace fq::cli
