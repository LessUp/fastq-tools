#include "fqtools/processing/processing_pipeline.h"
#include "fqtools/processing/mutators.h"
#include "fqtools/processing/predicates.h"
#include <gtest/gtest.h>

TEST(PipelineSmokeTest, CanCreatePipelineFromFactory) {
    auto pipeline = fq::processing::createProcessingPipeline();
    ASSERT_TRUE(static_cast<bool>(pipeline));
}
