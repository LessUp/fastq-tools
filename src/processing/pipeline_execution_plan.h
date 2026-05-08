#pragma once

#include "fqtools/processing/processing_options.h"

#include <cstddef>

#include "processing/runtime_policy.h"

namespace fq::processing {

enum class PipelineExecutionMode : std::uint8_t {
    Sequential,
    Parallel,
};

struct PipelineExecutionPlan {
    PipelineExecutionMode mode = PipelineExecutionMode::Sequential;
    size_t threadCount = 1;
    size_t maxLiveTokens = 1;
};

[[nodiscard]] auto derivePipelineExecutionPlan(const ProcessingOptions& options,
                                               const RuntimePolicy& runtimePolicy,
                                               bool hasCustomReader,
                                               bool hasCustomWriter) -> PipelineExecutionPlan;

}  // namespace fq::processing
