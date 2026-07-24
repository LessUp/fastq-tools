/**
 * @file enum_parser.h
 * @brief 统一的字符串-枚举转换工具
 * @details 提供类型安全的字符串到枚举的转换，消除分散的转换逻辑。
 *
 */

#pragma once

#include "fqtools/processing/mutators.h"
#include "fqtools/processing/processing_options.h"

#include <string>

namespace fq::cli {

/**
 * @brief 解析性能预设字符串
 * @param value 预设名称（default, lowMemory, highThroughput）
 * @return ProcessingProfile 枚举值
 * @throws std::invalid_argument 如果值无效
 */
[[nodiscard]] auto parseProcessingProfile(const std::string& value)
    -> processing::ProcessingProfile;

/**
 * @brief 解析修剪模式字符串
 * @param value 模式名称（five, three, both）
 * @return QualityTrimmer::TrimMode 枚举值
 * @throws std::invalid_argument 如果值无效
 */
[[nodiscard]] auto parseTrimMode(const std::string& value) -> processing::QualityTrimmer::TrimMode;

}  // namespace fq::cli
