#pragma once

#include "fqtools/processing/processing_options.h"

#include <cstddef>

namespace fq::processing {

struct RuntimePolicy {
    size_t readChunkBytes = 1 * 1024 * 1024;
    size_t zlibBufferBytes = 128 * 1024;
    size_t batchCapacityBytes = 4 * 1024 * 1024;
    size_t writerBufferBytes = 128 * 1024;
    size_t maxLiveTokens = 4;
    bool allocationTelemetryEnabled = false;
};

[[nodiscard]] auto deriveRuntimePolicy(const ProcessingOptions& options) -> RuntimePolicy;

}  // namespace fq::processing
