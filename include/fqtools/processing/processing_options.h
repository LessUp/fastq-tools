/**
 * @file processing_options.h
 * @brief 处理管道配置选项
 * @details 定义用户可见的处理选项和内部配置结构，实现配置分层：
 *          - ProcessingOptions: 用户可见的简明配置
 *          - ProcessingProfile: 预设性能配置
 *          - InternalConfig: 内部使用的详细配置
 *
 * @author LessUp
 * @date 2026-05-08
 * @version 2.0
 * @copyright (c) 2026 LessUp. All rights reserved.
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
    size_t batchSize = 10000;  ///< 批处理大小（每个批次的读取数量）
    size_t threadCount = 1;    ///< 线程数量（1 表示串行处理）
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

/**
 * @brief 内部使用的详细配置
 * @details 由 ProcessingOptions 和 ProcessingProfile 自动生成，
 *          不暴露给用户，仅在实现层使用。
 */
struct InternalConfig {
    /// @name 读取器配置
    /// @{
    size_t readChunkBytes = 1 * 1024 * 1024;  ///< 读取块大小（字节）
    size_t zlibBufferBytes = 128 * 1024;      ///< zlib 内部缓冲区大小
    /// @}

    /// @name 批处理配置
    /// @{
    size_t batchCapacityBytes = 4 * 1024 * 1024;  ///< 批次缓冲区容量（字节）
    /// @}

    /// @name 写入器配置
    /// @{
    size_t writerBufferBytes = 128 * 1024;  ///< 写入器缓冲区大小
    /// @}

    /// @name 并行配置
    /// @{
    size_t maxInFlightBatches = 0;  ///< 最大并行批次（0 表示自动）
    /// @}

    /// @name 调试配置
    /// @{
    bool allocationTelemetryEnabled = false;  ///< 是否启用内存遥测
    /// @}

    /**
     * @brief 从用户选项生成内部配置
     * @param opts 用户可见的处理选项
     * @return 内部配置实例
     */
    [[nodiscard]] static auto fromOptions(const ProcessingOptions& opts) -> InternalConfig;
};

}  // namespace fq::processing
