/**
 * @file processing_options.h
 * @brief 处理管道配置选项
 * @details 定义用户可见的处理选项和预设配置：
 *          - ProcessingOptions: 用户可见的简明配置
 *          - ProcessingProfile: 预设性能配置
 *
 * @author LessUp
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace fq::processing {

/**
 * @brief 性能预设枚举
 * @details 提供三种预设配置，覆盖常见使用场景
 */
enum class ProcessingProfile : std::uint8_t {
    Default,        ///< 平衡模式：适合大多数场景
    LowMemory,      ///< 内存受限场景：适合小内存机器或大规模并行任务
    HighThroughput  ///< 最大吞吐量：适合高性能服务器
};

/**
 * @brief 用户可见的处理选项
 * @details 提供简明的配置接口，隐藏底层实现细节。
 *          用户只需关注业务参数，内部参数由预设自动生成。
 *
 * @example
 * @code
 * ProcessingOptions opts;
 * opts.batchSize = 50000;
 * opts.threadCount = 8;
 * opts.profile = ProcessingProfile::HighThroughput;
 * opts.memoryLimitBytes = 16ULL * 1024 * 1024 * 1024;  // 可选：16GB 限制
 * opts.validate();
 * @endcode
 */
struct ProcessingOptions {
    /// @name 业务参数
    /// @{
    size_t batchSize = 10000;                                ///< 批处理大小（每个批次的读取数量）
    size_t threadCount = 1;                                  ///< 线程数量（1 表示串行处理）
    ProcessingProfile profile = ProcessingProfile::Default;  ///< 性能预设
    /// @}

    /// @name 高级覆盖（可选）
    /// @{
    std::optional<size_t> memoryLimitBytes;  ///< 内存限制（字节），可选覆盖预设
    /// @}

    /**
     * @brief 验证配置有效性
     * @throws std::invalid_argument 如果配置无效
     */
    void validate() const;
};

}  // namespace fq::processing
