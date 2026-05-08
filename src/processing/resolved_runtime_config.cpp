/**
 * @file resolved_runtime_config.cpp
 * @brief 统一运行时配置的实现
 * @details 合并 runtime_policy.cpp 和 pipeline_execution_plan.cpp 的逻辑
 *
 * @author LessUp
 * @date 2026-05-08
 * @version 2.0
 * @copyright (c) 2026 LessUp. All rights reserved.
 */

#include "processing/resolved_runtime_config.h"

#include <algorithm>

namespace fq::processing {

namespace {

constexpr size_t kDefaultReadChunkBytes = 1 * 1024 * 1024;
constexpr size_t kDefaultBatchCapacityBytes = 4 * 1024 * 1024;
constexpr size_t kDefaultWriterBufferBytes = 128 * 1024;
constexpr size_t kZlibBufferBytes = 128 * 1024;

auto applyProfileDefaults(ResolvedRuntimeConfig& config, ProcessingProfile profile) -> void {
    switch (profile) {
        case ProcessingProfile::Default:
            config.readChunkBytes = kDefaultReadChunkBytes;
            config.batchCapacityBytes = kDefaultBatchCapacityBytes;
            config.writerBufferBytes = kDefaultWriterBufferBytes;
            break;
        case ProcessingProfile::LowMemory:
            config.readChunkBytes = 256 * 1024;
            config.batchCapacityBytes = 1 * 1024 * 1024;
            config.writerBufferBytes = 64 * 1024;
            break;
        case ProcessingProfile::HighThroughput:
            config.readChunkBytes = 4 * 1024 * 1024;
            config.batchCapacityBytes = 16 * 1024 * 1024;
            config.writerBufferBytes = 256 * 1024;
            break;
    }
}

auto resolveMaxLiveTokens(size_t memoryLimitBytes,
                          size_t batchCapacityBytes,
                          size_t threadCount,
                          ProcessingProfile profile) -> size_t {
    // LowMemory preset uses fixed token count
    if (profile == ProcessingProfile::LowMemory) {
        return 2;
    }

    // Auto-calculate based on thread count
    size_t requested = std::max(static_cast<size_t>(4), threadCount * 2);

    // Apply memory cap if specified
    if (memoryLimitBytes == 0 || batchCapacityBytes == 0) {
        return std::max(static_cast<size_t>(1), requested);
    }

    const size_t cap = (memoryLimitBytes * 7 / 10) / batchCapacityBytes;
    if (cap == 0) {
        return 1;
    }

    return std::max(static_cast<size_t>(1), std::min(requested, cap));
}

}  // namespace

auto resolveRuntimeConfig(const ProcessingOptions& options,
                          bool hasCustomReader,
                          bool hasCustomWriter) -> ResolvedRuntimeConfig {
    options.validate();

    ResolvedRuntimeConfig config;
    config.zlibBufferBytes = kZlibBufferBytes;
    config.threadCount = std::max(static_cast<size_t>(1), options.threadCount);
    config.allocationTelemetryEnabled = false;

    // Apply profile defaults
    applyProfileDefaults(config, options.profile);

    // Resolve max live tokens
    config.maxLiveTokens = resolveMaxLiveTokens(options.memoryLimitBytes.value_or(0),
                                                config.batchCapacityBytes,
                                                config.threadCount,
                                                options.profile);

    // Determine execution mode
    // Parallel mode requires: no custom I/O, threadCount > 1
    if (!hasCustomReader && !hasCustomWriter && config.threadCount > 1) {
        config.executionMode = ExecutionMode::Parallel;
    } else {
        config.executionMode = ExecutionMode::Sequential;
    }

    return config;
}

}  // namespace fq::processing
