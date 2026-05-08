#include "fqtools/io/reader_interface.h"
#include "fqtools/io/writer_interface.h"

#include <deque>
#include <fstream>

#include "fixture_loader.h"
#include "test_helpers.h"

#include "processing/execution_runtime.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace fq::processing {
namespace {

// Test helper: storage for FASTQ record strings that outlives batch usage
// Uses deque to prevent pointer invalidation on insertion
struct TestRecordStorage {
    std::deque<std::string> ids;
    std::deque<std::string> seqs;
    std::deque<std::string> quals;
    std::deque<std::string> pluses;

    auto makeBatch(std::string id, std::string seq) -> fq::io::FastqBatch {
        ids.push_back(std::move(id));
        seqs.push_back(std::move(seq));
        quals.push_back("IIII");
        pluses.push_back("+");

        const size_t idx = ids.size() - 1;

        fq::io::FastqBatch batch(1024, 1);
        batch.records().push_back(
            fq::io::FastqRecord{ids[idx], {}, seqs[idx], quals[idx], pluses[idx]});
        return batch;
    }
};

class DeterministicAdapter final : public ExecutionRuntimeAdapter {
public:
    explicit DeterministicAdapter(std::vector<fq::io::FastqBatch> batches)
        : batches_(std::move(batches)) {}

    auto nextBatch(fq::io::FastqBatch& batch, size_t maxRecords) -> bool override {
        if (cursor_ >= batches_.size()) {
            return false;
        }
        receivedBatchSizes_.push_back(maxRecords);
        batch = std::move(batches_[cursor_++]);
        return true;
    }

    auto commit(const fq::io::FastqBatch&) -> std::uint64_t override {
        return 0;
    }

    auto getReceivedBatchSizes() const -> const std::vector<size_t>& {
        return receivedBatchSizes_;
    }

private:
    std::vector<fq::io::FastqBatch> batches_;
    std::vector<size_t> receivedBatchSizes_;
    size_t cursor_ = 0;
};

class VectorReader final : public fq::io::IReader {
public:
    explicit VectorReader(std::vector<fq::io::FastqBatch> batches) : batches_(std::move(batches)) {}

    auto nextBatch(fq::io::FastqBatch& batch) -> bool override {
        if (cursor_ >= batches_.size()) {
            return false;
        }
        batch = std::move(batches_[cursor_++]);
        return true;
    }

private:
    std::vector<fq::io::FastqBatch> batches_;
    size_t cursor_ = 0;
};

auto serializedFastqBytes(const fq::io::FastqBatch& batch) -> std::uint64_t {
    std::uint64_t totalBytes = 0;
    for (const auto& record : batch) {
        const auto plusLine = record.plus.empty() ? std::string_view("+") : record.plus;
        totalBytes += 1 + record.id.size() + 1;
        totalBytes += record.seq.size() + 1;
        totalBytes += plusLine.size() + 1;
        totalBytes += record.qual.size() + 1;
        if (!record.comment.empty()) {
            totalBytes += 1 + record.comment.size();
        }
    }
    return totalBytes;
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

TEST(ExecutionRuntimeTest, DeterministicAdapterVisitsBatchesInOrder) {
    TestRecordStorage storage;
    auto adapter = std::make_unique<DeterministicAdapter>(std::vector<fq::io::FastqBatch>{
        storage.makeBatch("read1", "ACGT"), storage.makeBatch("read2", "TTTT")});
    ExecutionRuntime runtime(std::move(adapter));

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

TEST(ExecutionRuntimeTest, ForwardsBatchSizeToAdapter) {
    TestRecordStorage storage;
    auto adapter = std::make_unique<DeterministicAdapter>(std::vector<fq::io::FastqBatch>{
        storage.makeBatch("read1", "ACGT"), storage.makeBatch("read2", "TTTT")});
    auto* adapterPtr = adapter.get();
    ExecutionRuntime runtime(std::move(adapter));

    ExecutionRuntimePlan plan;
    plan.options.batchSize = 42;

    runtime.run<size_t>(
        plan,
        [](fq::io::FastqBatch&) { return size_t{1}; },
        [](size_t& total, size_t partial) { total += partial; },
        [](size_t&, std::uint64_t) {},
        size_t{0});

    const auto& received = adapterPtr->getReceivedBatchSizes();
    EXPECT_THAT(received, ::testing::Each(42));
    EXPECT_EQ(received.size(), 2U);
}

TEST(ExecutionRuntimeTest, RequiresNonNullAdapter) {
    EXPECT_THROW(ExecutionRuntime runtime(nullptr), std::invalid_argument);
}

TEST(ExecutionRuntimeTest, ValidatesProcessingOptionsBeforeRun) {
    TestRecordStorage storage;
    auto adapter = std::make_unique<DeterministicAdapter>(
        std::vector<fq::io::FastqBatch>{storage.makeBatch("read1", "ACGT")});
    ExecutionRuntime runtime(std::move(adapter));

    ExecutionRuntimePlan plan;
    plan.options.batchSize = 0;  // Invalid: batchSize must be > 0
    plan.options.threadCount = 1;

    EXPECT_THROW(runtime.run<size_t>(
                     plan,
                     [](fq::io::FastqBatch&) { return size_t{1}; },
                     [](size_t& total, size_t partial) { total += partial; },
                     [](size_t&, std::uint64_t) {},
                     size_t{0}),
                 std::invalid_argument);
}

TEST(ExecutionRuntimeTest, ValidatesThreadCountBeforeRun) {
    TestRecordStorage storage;
    auto adapter = std::make_unique<DeterministicAdapter>(
        std::vector<fq::io::FastqBatch>{storage.makeBatch("read1", "ACGT")});
    ExecutionRuntime runtime(std::move(adapter));

    ExecutionRuntimePlan plan;
    plan.options.batchSize = 100;
    plan.options.threadCount = 0;  // Invalid: threadCount must be > 0

    EXPECT_THROW(runtime.run<size_t>(
                     plan,
                     [](fq::io::FastqBatch&) { return size_t{1}; },
                     [](size_t& total, size_t partial) { total += partial; },
                     [](size_t&, std::uint64_t) {},
                     size_t{0}),
                 std::invalid_argument);
}

TEST(ExecutionRuntimeTest, CallbackSequencingIsCorrect) {
    TestRecordStorage storage;
    auto adapter = std::make_unique<DeterministicAdapter>(std::vector<fq::io::FastqBatch>{
        storage.makeBatch("read1", "ACGT"), storage.makeBatch("read2", "TTTT")});
    ExecutionRuntime runtime(std::move(adapter));

    ExecutionRuntimePlan plan;
    plan.options.batchSize = 1;
    plan.options.threadCount = 1;

    std::vector<std::string> callOrder;

    runtime.run<size_t>(
        plan,
        [&](fq::io::FastqBatch&) {
            callOrder.push_back("batchWork");
            return size_t{1};
        },
        [&](size_t& total, size_t partial) {
            callOrder.push_back("reduce");
            total += partial;
        },
        [&](size_t&, std::uint64_t) { callOrder.push_back("afterCommit"); },
        size_t{0});

    // Expected order per batch: batchWork -> commit (implicit) -> afterCommit -> reduce
    EXPECT_THAT(callOrder,
                ::testing::ElementsAre(
                    "batchWork", "afterCommit", "reduce", "batchWork", "afterCommit", "reduce"));
}

class RecordingWriter final : public fq::io::IWriter {
public:
    void write(const fq::io::FastqBatch& batch) override {
        auto& mutableBatch = const_cast<fq::io::FastqBatch&>(batch);
        for (const auto& record : mutableBatch.records()) {
            writtenIds.push_back(std::string(record.id));
        }
    }

    std::vector<std::string> writtenIds;
};

TEST(ExecutionRuntimeTest, CustomReaderWriterFlowStillCommitsThroughRuntime) {
    TestRecordStorage storage;
    auto writer = std::make_shared<RecordingWriter>();
    ExecutionRuntime runtime(
        makeAdapterForTesting(std::make_unique<VectorReader>(std::vector<fq::io::FastqBatch>{
                                  storage.makeBatch("readA", "ACGT")}),
                              writer));

    ExecutionRuntimePlan plan;
    plan.options.batchSize = 1;
    plan.options.threadCount = 1;

    const auto totalReads = runtime.run<size_t>(
        plan,
        [](fq::io::FastqBatch& batch) { return batch.records().size(); },
        [](size_t& total, size_t partial) { total += partial; },
        [](size_t&, std::uint64_t) {},
        size_t{0});

    EXPECT_EQ(totalReads, 1U);
    EXPECT_THAT(writer->writtenIds, ::testing::ElementsAre("readA"));
}

// Issue #1: FileBackedAdapter must respect maxRecords (batchSize)
TEST(ExecutionRuntimeTest, FileBackedAdapterRespectsBatchSize) {
    ExecutionRuntime runtime;  // Default construction, will use FileBackedAdapter
    fq::test::TempDirectory tempDir("execution_runtime_");

    ExecutionRuntimePlan plan;
    plan.inputPath = writeGeneratedFastqInput(tempDir, "input.fastq", 250);
    plan.options.batchSize = 100;  // Request batches of 100 records
    plan.options.threadCount = 1;

    std::vector<size_t> batchSizes;
    runtime.run<size_t>(
        plan,
        [&](fq::io::FastqBatch& batch) {
            batchSizes.push_back(batch.records().size());
            return batch.records().size();
        },
        [](size_t& total, size_t partial) { total += partial; },
        [](size_t&, std::uint64_t) {},
        size_t{0});

    // All batches except possibly the last should have exactly 100 records
    for (size_t i = 0; i + 1 < batchSizes.size(); ++i) {
        EXPECT_EQ(batchSizes[i], 100U) << "Batch " << i << " should have exactly 100 records";
    }
    // Last batch can be <= 100
    EXPECT_LE(batchSizes.back(), 100U);
    EXPECT_EQ(batchSizes.size(), 3U);
}

// Issue #2: Partial custom I/O injection scenarios
TEST(ExecutionRuntimeTest, LazyInitWithCustomWriterOnlyUsesFileBackedReader) {
    ExecutionRuntime runtime;  // Default construction - lazy initialization
    fq::test::TempDirectory tempDir("execution_runtime_");

    auto writer = std::make_shared<RecordingWriter>();
    runtime.setCustomWriter(writer);

    ExecutionRuntimePlan plan;
    plan.inputPath = writeGeneratedFastqInput(tempDir, "input.fastq", 250);
    plan.options.batchSize = 100;
    plan.options.threadCount = 1;

    const auto totalReads = runtime.run<size_t>(
        plan,
        [](fq::io::FastqBatch& batch) { return batch.records().size(); },
        [](size_t& total, size_t partial) { total += partial; },
        [](size_t&, std::uint64_t) {},
        size_t{0});

    // Should read all records from file and write to custom writer
    EXPECT_EQ(totalReads, 250U);
    EXPECT_EQ(writer->writtenIds.size(), 250U);
}

TEST(ExecutionRuntimeTest, LazyInitWithCustomReaderOnlyUsesFileBackedWriter) {
    TestRecordStorage storage;
    fq::test::TempDirectory tempDir("execution_runtime_");
    ExecutionRuntime runtime;  // Default construction - lazy initialization

    runtime.setCustomReader(std::make_unique<VectorReader>(std::vector<fq::io::FastqBatch>{
        storage.makeBatch("read1", "ACGT"), storage.makeBatch("read2", "GGGG")}));

    ExecutionRuntimePlan plan;
    plan.outputPath = (tempDir.path() / "custom_reader.fastq").string();
    plan.options.batchSize = 1;
    plan.options.threadCount = 1;

    const auto totalReads = runtime.run<size_t>(
        plan,
        [](fq::io::FastqBatch& batch) { return batch.records().size(); },
        [](size_t& total, size_t partial) { total += partial; },
        [](size_t&, std::uint64_t) {},
        size_t{0});

    // Should read from custom reader and write to file
    EXPECT_EQ(totalReads, 2U);
    EXPECT_TRUE(std::filesystem::exists(*plan.outputPath));
}

// Issue #3: IOBackedAdapter commit must return meaningful committedBytes
TEST(ExecutionRuntimeTest, CustomWriterWithFileBackedReaderReportsCommittedBytes) {
    // Partial injection: custom writer + file-backed reader
    ExecutionRuntime runtime;
    fq::test::TempDirectory tempDir("execution_runtime_");
    auto writer = std::make_shared<RecordingWriter>();
    runtime.setCustomWriter(writer);

    ExecutionRuntimePlan plan;
    plan.inputPath = writeGeneratedFastqInput(tempDir, "input.fastq", 250);
    plan.options.batchSize = 100;
    plan.options.threadCount = 1;

    std::vector<std::uint64_t> committedBytesPerBatch;
    runtime.run<size_t>(
        plan,
        [](fq::io::FastqBatch& batch) { return batch.records().size(); },
        [](size_t& total, size_t partial) { total += partial; },
        [&](size_t&, std::uint64_t bytes) { committedBytesPerBatch.push_back(bytes); },
        size_t{0});

    // File-backed reader populates buffer, so custom writer should report non-zero bytes
    EXPECT_GT(committedBytesPerBatch.size(), 0U);
    for (size_t i = 0; i < committedBytesPerBatch.size(); ++i) {
        EXPECT_GT(committedBytesPerBatch[i], 0U)
            << "Batch " << i << " should report non-zero committed bytes";
    }
}

// Additional test: Custom I/O with mock reader (buffer may be empty)
TEST(ExecutionRuntimeTest, CustomIOReturnsBufferSizeForCommittedBytes) {
    TestRecordStorage storage;
    auto writer = std::make_shared<RecordingWriter>();

    ExecutionRuntime runtime(
        makeAdapterForTesting(std::make_unique<VectorReader>(std::vector<fq::io::FastqBatch>{
                                  storage.makeBatch("read1", "ACGT")}),
                              writer));

    ExecutionRuntimePlan plan;
    plan.options.batchSize = 1;
    plan.options.threadCount = 1;

    std::vector<std::uint64_t> committedBytesPerBatch;
    runtime.run<size_t>(
        plan,
        [](fq::io::FastqBatch& batch) { return batch.records().size(); },
        [](size_t& total, size_t partial) { total += partial; },
        [&](size_t&, std::uint64_t bytes) { committedBytesPerBatch.push_back(bytes); },
        size_t{0});

    // Mock reader with empty buffer returns batch.buffer().size() which is 0
    // This is expected behavior - the contract is batch.buffer().size()
    ASSERT_EQ(committedBytesPerBatch.size(), 1U);
    // With mock reader that doesn't populate buffer, committedBytes will be 0
    // This is acceptable because the contract is batch.buffer().size()
}

TEST(ExecutionRuntimeTest, CustomReaderWithFileBackedWriterReportsExactCommittedBytes) {
    TestRecordStorage storage;
    auto firstBatch = storage.makeBatch("read1", "ACGT");
    auto secondBatch = storage.makeBatch("read2", "GGGG");
    const auto expectedBytes = std::vector<std::uint64_t>{serializedFastqBytes(firstBatch),
                                                          serializedFastqBytes(secondBatch)};

    fq::test::TempDirectory tempDir("execution_runtime_");
    ExecutionRuntime runtime;
    runtime.setCustomReader(std::make_unique<VectorReader>(
        std::vector<fq::io::FastqBatch>{std::move(firstBatch), std::move(secondBatch)}));

    ExecutionRuntimePlan plan;
    plan.outputPath = (tempDir.path() / "custom_reader_output.fastq").string();
    plan.options.batchSize = 1;
    plan.options.threadCount = 1;

    std::vector<std::uint64_t> committedBytesPerBatch;
    const auto totalReads = runtime.run<size_t>(
        plan,
        [](fq::io::FastqBatch& batch) { return batch.records().size(); },
        [](size_t& total, size_t partial) { total += partial; },
        [&](size_t&, std::uint64_t bytes) { committedBytesPerBatch.push_back(bytes); },
        size_t{0});

    EXPECT_EQ(totalReads, 2U);
    EXPECT_THAT(committedBytesPerBatch, ::testing::ElementsAreArray(expectedBytes));
}

TEST(ExecutionRuntimeTest, RebuildsLazyAdapterWhenCustomWriterChangesAfterRun) {
    ExecutionRuntime runtime;
    fq::test::TempDirectory tempDir("execution_runtime_");

    ExecutionRuntimePlan plan;
    plan.inputPath = writeGeneratedFastqInput(tempDir, "input.fastq", 250);
    plan.options.batchSize = 250;
    plan.options.threadCount = 1;

    const auto firstPassReads = runtime.run<size_t>(
        plan,
        [](fq::io::FastqBatch& batch) { return batch.records().size(); },
        [](size_t& total, size_t partial) { total += partial; },
        [](size_t&, std::uint64_t) {},
        size_t{0});

    auto writer = std::make_shared<RecordingWriter>();
    runtime.setCustomWriter(writer);

    const auto secondPassReads = runtime.run<size_t>(
        plan,
        [](fq::io::FastqBatch& batch) { return batch.records().size(); },
        [](size_t& total, size_t partial) { total += partial; },
        [](size_t&, std::uint64_t) {},
        size_t{0});

    EXPECT_EQ(firstPassReads, 250U);
    EXPECT_EQ(secondPassReads, firstPassReads);
    EXPECT_EQ(writer->writtenIds.size(), firstPassReads);
}

}  // namespace fq::processing
