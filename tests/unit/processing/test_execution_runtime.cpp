#include "processing/execution_runtime.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace fq::processing {
namespace {

auto makeBatch(std::string id, std::string seq) -> fq::io::FastqBatch {
    fq::io::FastqBatch batch(1024, 1);
    batch.records().push_back(fq::io::FastqRecord{std::move(id), {}, std::move(seq), "IIII", "+"});
    return batch;
}

class DeterministicAdapter final : public ExecutionRuntimeAdapter {
public:
    explicit DeterministicAdapter(std::vector<fq::io::FastqBatch> batches)
        : batches_(std::move(batches)) {}

    auto nextBatch(fq::io::FastqBatch& batch, size_t /*maxRecords*/) -> bool override {
        if (cursor_ >= batches_.size()) {
            return false;
        }
        batch = std::move(batches_[cursor_++]);
        return true;
    }

    auto commit(const fq::io::FastqBatch&) -> std::uint64_t override { return 0; }

private:
    std::vector<fq::io::FastqBatch> batches_;
    size_t cursor_ = 0;
};

}  // namespace

TEST(ExecutionRuntimeTest, DeterministicAdapterVisitsBatchesInOrder) {
    ExecutionRuntime runtime(std::make_unique<DeterministicAdapter>(
        std::vector<fq::io::FastqBatch>{makeBatch("read1", "ACGT"), makeBatch("read2", "TTTT")}));

    ExecutionRuntimePlan plan;
    plan.options.batchSize = 1;
    plan.options.threadCount = 2;

    std::vector<std::string> seenIds;
    const auto totalReads = runtime.run<size_t>(
        plan,
        [&](fq::io::FastqBatch& batch) {
            seenIds.push_back(std::string(batch.records().front().id));
            return batch.records().size();
        },
        [](size_t& total, size_t partial) { total += partial; },
        [](size_t&, std::uint64_t) {},
        size_t{0});

    EXPECT_EQ(totalReads, 2U);
    EXPECT_THAT(seenIds, ::testing::ElementsAre("read1", "read2"));
}

}  // namespace fq::processing
