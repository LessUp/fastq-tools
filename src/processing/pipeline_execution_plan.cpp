#include "processing/pipeline_execution_plan.h"

#include <algorithm>

namespace fq::processing {

auto derivePipelineExecutionPlan(const ProcessingOptions& options,
                                 const RuntimePolicy& runtimePolicy,
                                 bool hasCustomReader,
                                 bool hasCustomWriter) -> PipelineExecutionPlan {
    PipelineExecutionPlan plan;
    plan.threadCount = std::max(static_cast<size_t>(1), options.threadCount);
    plan.maxLiveTokens = runtimePolicy.maxLiveTokens;

    if (!hasCustomReader && !hasCustomWriter && plan.threadCount > 1) {
        plan.mode = PipelineExecutionMode::Parallel;
    }

    return plan;
}

}  // namespace fq::processing
