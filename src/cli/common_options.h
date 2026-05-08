/**
 * @file common_options.h
 * @brief CLI 共享参数定义
 * @details 定义 stat 和 filter 命令共享的 CLI 参数，
 *          消除重复代码，提高维护性。
 *
 * @author LessUp
 * @date 2026-05-08
 * @version 2.0
 * @copyright (c) 2026 LessUp. All rights reserved.
 */

#pragma once

#include "fqtools/processing/processing_options.h"

#include <cstddef>
#include <optional>
#include <string>

// 前向声明
namespace cxxopts {
class Options;
class ParseResult;
}  // namespace cxxopts

namespace fq::cli {

/**
 * @brief 共享 CLI 参数结构体
 * @details 封装 stat 和 filter 命令的共同参数
 */
struct CommonCliOptions {
    /// @name 基本参数
    /// @{
    std::string inputPath;     ///< 输入文件路径
    std::string outputPath;    ///< 输出文件路径
    size_t threadCount = 1;    ///< 线程数
    size_t batchSize = 10000;  ///< 批处理大小
    processing::ProcessingProfile profile = processing::ProcessingProfile::Default;  ///< 性能预设
    /// @}

    /// @name 高级参数（可选）
    /// @{
    std::optional<size_t> memoryLimitGb;  ///< 内存限制（GB）
    /// @}

    /**
     * @brief 添加共享参数到 cxxopts
     * @param options cxxopts::Options 实例
     */
    static void addOptions(cxxopts::Options& options);

    /**
     * @brief 从解析结果提取共享参数
     * @param result cxxopts 解析结果
     * @return CommonCliOptions 实例
     */
    [[nodiscard]] static auto parse(const cxxopts::ParseResult& result) -> CommonCliOptions;

    /**
     * @brief 转换为 ProcessingOptions
     * @return ProcessingOptions 实例
     */
    [[nodiscard]] auto toProcessingOptions() const -> processing::ProcessingOptions;
};

/**
 * @brief 解析性能预设字符串
 * @param value 预设名称（default, lowMemory, highThroughput）
 * @return ProcessingProfile 枚举值
 * @throws std::invalid_argument 如果值无效
 */
[[nodiscard]] auto parseProcessingProfile(const std::string& value)
    -> processing::ProcessingProfile;

}  // namespace fq::cli
