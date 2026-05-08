#include "processing/runtime_policy.h"
#include <fqtools/processing/processing_options.h>
#include <gtest/gtest.h>

namespace fq::processing {

TEST(RuntimePolicyTest, DerivesMemoryBoundedLiveTokenCountFromProcessingOptions) {
    ProcessingOptions options;
    options.threadCount = 4;
    options.memoryLimitBytes = 8ULL * 1024ULL * 1024ULL;

    const auto policy = deriveRuntimePolicy(options);

    EXPECT_EQ(policy.readChunkBytes, 1ULL * 1024ULL * 1024ULL);
    EXPECT_EQ(policy.batchCapacityBytes, 4ULL * 1024ULL * 1024ULL);
    EXPECT_EQ(policy.writerBufferBytes, 128ULL * 1024ULL);
    EXPECT_EQ(policy.maxLiveTokens, 1U);
}

}  // namespace fq::processing
