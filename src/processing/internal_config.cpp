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

#include "processing/runtime_policy.h"

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
    const auto policy = deriveRuntimePolicy(opts);

    InternalConfig config;
    config.readChunkBytes = policy.readChunkBytes;
    config.zlibBufferBytes = policy.zlibBufferBytes;
    config.batchCapacityBytes = policy.batchCapacityBytes;
    config.writerBufferBytes = policy.writerBufferBytes;
    config.maxInFlightBatches = policy.maxLiveTokens;
    config.allocationTelemetryEnabled = policy.allocationTelemetryEnabled;

    return config;
}

auto calculateMaxInFlightBatches(size_t memoryLimitBytes,
                                 size_t batchCapacityBytes,
                                 size_t threadCount) -> size_t {
    if (memoryLimitBytes == 0 || batchCapacityBytes == 0) {
        return std::max(static_cast<size_t>(1), std::max(static_cast<size_t>(4), threadCount * 2));
    }

    const size_t requested = std::max(static_cast<size_t>(4), threadCount * 2);
    const size_t cap = (memoryLimitBytes * 7 / 10) / batchCapacityBytes;
    if (cap == 0) {
        return 1;
    }

    return std::max(static_cast<size_t>(1), std::min(requested, cap));
}

}  // namespace fq::processing
