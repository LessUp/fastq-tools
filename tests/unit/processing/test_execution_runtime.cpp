#include <filesystem>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

#include "fixture_loader.h"
#include "test_helpers.h"

#include "processing/execution_runtime.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace fq::processing {
namespace {

struct BatchSummary {
    std::vector<std::string> firstIds;
    std::vector<size_t> batchSizes;
    std::vector<std::uint64_t> committedBytes;
    size_t totalReads = 0;
};

class RecordingCommandAdapter {
public:
    using result_type = BatchSummary;

    auto makeResult() const -> result_type {
        return {};
    }

    auto processBatch(fq::io::FastqBatch& batch) const -> result_type {
        result_type partial;
        partial.batchSizes.push_back(batch.records().size());
        partial.totalReads = batch.records().size();
        if (!batch.records().empty()) {
            partial.firstIds.push_back(std::string(batch.records().front().id));
        }
        return partial;
    }

    void afterCommit(result_type& partial, std::uint64_t committedBytes) const {
        partial.committedBytes.push_back(committedBytes);
    }

    void merge(result_type& total, result_type partial) const {
        total.totalReads += partial.totalReads;
        total.firstIds.insert(
            total.firstIds.end(), partial.firstIds.begin(), partial.firstIds.end());
        total.batchSizes.insert(
            total.batchSizes.end(), partial.batchSizes.begin(), partial.batchSizes.end());
        total.committedBytes.insert(total.committedBytes.end(),
                                    partial.committedBytes.begin(),
                                    partial.committedBytes.end());
    }
};

auto writeFastqInput(fq::test::TempDirectory& tempDir,
                     std::string_view filename,
                     std::string_view content) -> std::string {
    const auto path = tempDir.path() / filename;
    std::ofstream out(path);
    out << content;
    return path.string();
}

auto writeGeneratedFastqInput(fq::test::TempDirectory& tempDir,
                              std::string_view filename,
                              size_t recordCount) -> std::string {
    const auto path = tempDir.path() / filename;
    std::ofstream out(path);
    out << fq::test::TestDataGenerator::generateFastQRecords(recordCount, 20);
    return path.string();
}

}  // namespace

TEST(ExecutionRuntimeTest, ExecuteProcessesBatchesThroughSingleAdapter) {
    fq::test::TempDirectory tempDir("execution_runtime_");
    ExecutionRuntime runtime;
    RecordingCommandAdapter adapter;

    ExecutionRuntimeRequest request;
    request.inputPath = writeFastqInput(tempDir,
                                        "input.fastq",
                                        "@read1\nACGT\n+\nIIII\n"
                                        "@read2\nTTTT\n+\n####\n");
    request.options.batchSize = 1;
    request.options.threadCount = 1;

    const auto outcome = runtime.execute(request, adapter);

    EXPECT_EQ(outcome.result.totalReads, 2U);
    EXPECT_EQ(outcome.metrics.batchCount, 2U);
    EXPECT_THAT(outcome.result.firstIds, ::testing::ElementsAre("read1", "read2"));
    EXPECT_THAT(outcome.result.batchSizes, ::testing::ElementsAre(1U, 1U));
}

TEST(ExecutionRuntimeTest, ExecuteRespectsConfiguredBatchSize) {
    fq::test::TempDirectory tempDir("execution_runtime_");
    ExecutionRuntime runtime;
    RecordingCommandAdapter adapter;

    ExecutionRuntimeRequest request;
    request.inputPath = writeGeneratedFastqInput(tempDir, "input.fastq", 250);
    request.options.batchSize = 100;
    request.options.threadCount = 1;

    const auto outcome = runtime.execute(request, adapter);

    EXPECT_EQ(outcome.result.totalReads, 250U);
    EXPECT_THAT(outcome.result.batchSizes, ::testing::ElementsAre(100U, 100U, 50U));
}

TEST(ExecutionRuntimeTest, ExecuteReportsCommittedBytesForCommittedBatches) {
    fq::test::TempDirectory tempDir("execution_runtime_");
    ExecutionRuntime runtime;
    RecordingCommandAdapter adapter;

    ExecutionRuntimeRequest request;
    request.inputPath = writeFastqInput(tempDir,
                                        "input.fastq",
                                        "@read1\nACGT\n+\nIIII\n"
                                        "@read2\nGGGG\n+\nIIII\n");
    request.outputPath = (tempDir.path() / "output.fastq").string();
    request.options.batchSize = 1;
    request.options.threadCount = 1;

    const auto outcome = runtime.execute(request, adapter);
    const auto committedTotal = std::accumulate(outcome.result.committedBytes.begin(),
                                                outcome.result.committedBytes.end(),
                                                std::uint64_t{0});

    ASSERT_EQ(outcome.result.committedBytes.size(), 2U);
    EXPECT_THAT(outcome.result.committedBytes, ::testing::Each(::testing::Gt(0U)));
    EXPECT_EQ(outcome.metrics.committedBytes, committedTotal);
    EXPECT_TRUE(std::filesystem::exists(*request.outputPath));
    EXPECT_EQ(outcome.metrics.committedBytes, std::filesystem::file_size(*request.outputPath));
}

TEST(ExecutionRuntimeTest, ExecuteValidatesProcessingOptionsBeforeTraversal) {
    fq::test::TempDirectory tempDir("execution_runtime_");
    ExecutionRuntime runtime;
    RecordingCommandAdapter adapter;

    ExecutionRuntimeRequest request;
    request.inputPath = writeFastqInput(tempDir, "input.fastq", "@read1\nACGT\n+\nIIII\n");
    request.options.batchSize = 0;
    request.options.threadCount = 1;

    EXPECT_THROW(static_cast<void>(runtime.execute(request, adapter)), std::invalid_argument);
}

TEST(ExecutionRuntimeTest, ExecuteKeepsBatchCommitOrderInParallelMode) {
    fq::test::TempDirectory tempDir("execution_runtime_");
    ExecutionRuntime runtime;
    RecordingCommandAdapter adapter;

    ExecutionRuntimeRequest request;
    request.inputPath = writeFastqInput(tempDir,
                                        "input.fastq",
                                        "@read1\nACGT\n+\nIIII\n"
                                        "@read2\nTTTT\n+\nIIII\n");
    request.outputPath = (tempDir.path() / "output.fastq").string();
    request.options.batchSize = 1;
    request.options.threadCount = 2;

    const auto outcome = runtime.execute(request, adapter);

    EXPECT_EQ(outcome.metrics.batchCount, 2U);
    EXPECT_THAT(outcome.result.firstIds, ::testing::ElementsAre("read1", "read2"));
    EXPECT_THAT(outcome.result.committedBytes, ::testing::Each(::testing::Gt(0U)));
}

}  // namespace fq::processing
