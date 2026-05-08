#include "processing/runtime_policy.h"

#include <algorithm>

namespace fq::processing {

namespace {

constexpr size_t kDefaultReadChunkBytes = 1 * 1024 * 1024;
constexpr size_t kDefaultBatchCapacityBytes = 4 * 1024 * 1024;
constexpr size_t kDefaultWriterBufferBytes = 128 * 1024;
constexpr size_t kZlibBufferBytes = 128 * 1024;

auto resolveConfiguredTokens(const ProcessingOptions& options) -> size_t {
    switch (options.profile) {
        case ProcessingProfile::Default:
            return 0;
        case ProcessingProfile::LowMemory:
            return 2;
        case ProcessingProfile::HighThroughput:
            return 0;
    }

    return 0;
}

auto applyProfileDefaults(RuntimePolicy& policy, ProcessingProfile profile) -> void {
    switch (profile) {
        case ProcessingProfile::Default:
            policy.readChunkBytes = kDefaultReadChunkBytes;
            policy.batchCapacityBytes = kDefaultBatchCapacityBytes;
            policy.writerBufferBytes = kDefaultWriterBufferBytes;
            break;
        case ProcessingProfile::LowMemory:
            policy.readChunkBytes = 256 * 1024;
            policy.batchCapacityBytes = 1 * 1024 * 1024;
            policy.writerBufferBytes = 64 * 1024;
            break;
        case ProcessingProfile::HighThroughput:
            policy.readChunkBytes = 4 * 1024 * 1024;
            policy.batchCapacityBytes = 16 * 1024 * 1024;
            policy.writerBufferBytes = 256 * 1024;
            break;
    }
}

auto resolveAutoTokens(size_t threadCount) -> size_t {
    return std::max(static_cast<size_t>(4), threadCount * 2);
}

auto applyMemoryCap(size_t requestedTokens,
                    size_t memoryLimitBytes,
                    size_t batchCapacityBytes) -> size_t {
    if (memoryLimitBytes == 0 || batchCapacityBytes == 0) {
        return std::max(static_cast<size_t>(1), requestedTokens);
    }

    const size_t cap = (memoryLimitBytes * 7 / 10) / batchCapacityBytes;
    if (cap == 0) {
        return 1;
    }

    return std::max(static_cast<size_t>(1), std::min(requestedTokens, cap));
}

}  // namespace

auto deriveRuntimePolicy(const ProcessingOptions& options) -> RuntimePolicy {
    options.validate();

    RuntimePolicy policy;
    policy.zlibBufferBytes = kZlibBufferBytes;
    applyProfileDefaults(policy, options.profile);

    size_t requestedTokens = resolveConfiguredTokens(options);
    if (requestedTokens == 0) {
        requestedTokens = resolveAutoTokens(std::max(static_cast<size_t>(1), options.threadCount));
    }

    policy.maxLiveTokens = applyMemoryCap(
        requestedTokens, options.memoryLimitBytes.value_or(0), policy.batchCapacityBytes);
    return policy;
}

}  // namespace fq::processing
