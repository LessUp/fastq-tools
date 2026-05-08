#include "processing/pipeline_execution_plan.h"
#include "processing/runtime_policy.h"
#include <fqtools/processing/processing_options.h>
#include <gtest/gtest.h>

namespace fq::processing {

TEST(PipelineExecutionPlanTest, FallsBackToSequentialModeWhenCustomIoIsPresent) {
    ProcessingOptions options;
    options.threadCount = 4;

    const auto runtimePolicy = deriveRuntimePolicy(options);
    const auto plan = derivePipelineExecutionPlan(
        options, runtimePolicy, /*hasCustomReader*/ true, /*hasCustomWriter*/ false);

    EXPECT_EQ(plan.mode, PipelineExecutionMode::Sequential);
    EXPECT_EQ(plan.maxLiveTokens, runtimePolicy.maxLiveTokens);
}

}  // namespace fq::processing
