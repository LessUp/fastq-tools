/**
 * @file internal_config.cpp
 * @brief 内部配置实现
 * @details 实现 InternalConfig::fromOptions 预设映射逻辑
 *
 * @author LessUp
 * @date 2026-05-08
 * @version 2.0
 * @copyright (c) 2026 LessUp. All rights reserved.
 */

#include "processing/internal_config.h"

#include <algorithm>
#include <stdexcept>

namespace fq::processing {

void ProcessingOptions::validate() const {
    if (batchSize == 0) {
        throw std::invalid_argument("batchSize must be greater than 0");
    }
    if (threadCount == 0) {
        throw std::invalid_argument("threadCount must be greater than 0");
    }
    if (memoryLimitBytes.has_value() && memoryLimitBytes.value() == 0) {
        throw std::invalid_argument("memoryLimitBytes must be greater than 0 if specified");
    }
}

auto InternalConfig::fromOptions(const ProcessingOptions& opts) -> InternalConfig {
    InternalConfig config;

    // 根据预设设置基础配置
    switch (opts.profile) {
        case ProcessingProfile::Default:
            // 平衡模式：适合大多数场景
            config.readChunkBytes = 1 * 1024 * 1024;       // 1 MB
            config.batchCapacityBytes = 4 * 1024 * 1024;   // 4 MB
            config.writerBufferBytes = 128 * 1024;         // 128 KB
            config.maxInFlightBatches = 0;                 // 自动（基于线程数）
            break;

        case ProcessingProfile::LowMemory:
            // 内存受限场景：减小缓冲区，限制并发
            config.readChunkBytes = 256 * 1024;            // 256 KB
            config.batchCapacityBytes = 1 * 1024 * 1024;   // 1 MB
            config.writerBufferBytes = 64 * 1024;          // 64 KB
            config.maxInFlightBatches = 2;                 // 限制并行批次
            break;

        case ProcessingProfile::HighThroughput:
            // 最大吞吐量：增大缓冲区，提高并发
            config.readChunkBytes = 4 * 1024 * 1024;       // 4 MB
            config.batchCapacityBytes = 16 * 1024 * 1024;  // 16 MB
            config.writerBufferBytes = 256 * 1024;         // 256 KB
            config.maxInFlightBatches = 0;                 // 自动（更多）
            break;
    }

    // 固定配置
    config.zlibBufferBytes = 128 * 1024;
    config.allocationTelemetryEnabled = false;

    // 用户显式覆盖内存限制
    if (opts.memoryLimitBytes.has_value()) {
        config.maxInFlightBatches = calculateMaxInFlightBatches(
            opts.memoryLimitBytes.value(), config.batchCapacityBytes, opts.threadCount);
    }

    return config;
}

auto calculateMaxInFlightBatches(size_t memoryLimitBytes,
                                  size_t batchCapacityBytes,
                                  size_t threadCount) -> size_t {
    if (memoryLimitBytes == 0 || batchCapacityBytes == 0) {
        return 0;  // 自动
    }

    // 计算可容纳的最大批次数
    // 留 20% 余量给其他内存使用
    constexpr double memoryReserveRatio = 0.8;
    size_t usableMemory = static_cast<size_t>(memoryLimitBytes * memoryReserveRatio);
    size_t maxBatches = usableMemory / batchCapacityBytes;

    // 确保至少有线程数 + 2 个批次（避免饥饿）
    size_t minBatches = threadCount + 2;

    return std::max(maxBatches, minBatches);
}

}  // namespace fq::processing
