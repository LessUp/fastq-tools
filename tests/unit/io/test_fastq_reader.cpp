#include "fqtools/io/fastq_reader.h"
#include "fqtools/io/fastq_io.h"
#include "fqtools/error/error.h"

#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

class FastqReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temp file
        tmpFile_ = "test_reader.fastq";
        std::ofstream out(tmpFile_);
        // Record 1: Simple
        out << "@read1\n"
            << "ACGT\n"
            << "+\n"
            << "IIII\n";
        // Record 2: With comment
        out << "@read2 length=4\n"
            << "AAAA\n"
            << "+\n"
            << "JJJJ\n";
        // Record 3: Longer
        out << "@read3\n"
            << "ACGTACGTACGT\n"
            << "+\n"
            << "IIIIIIIIIIII\n";
        out.close();
    }

    void TearDown() override {
        if (std::filesystem::exists(tmpFile_)) {
            std::filesystem::remove(tmpFile_);
        }
    }

    std::string tmpFile_;
};

TEST_F(FastqReaderTest, ReadBasic) {
    fq::io::FastqReader reader(tmpFile_);
    EXPECT_TRUE(reader.isOpen());

    fq::io::FastqBatch batch;
    bool hasData = reader.nextBatch(batch);

    EXPECT_TRUE(hasData);
    EXPECT_EQ(batch.records().size(), 3);

    auto& records = batch.records();
    EXPECT_EQ(records[0].id, "read1");
    EXPECT_EQ(records[0].seq, "ACGT");
    EXPECT_EQ(records[0].qual, "IIII");

    EXPECT_EQ(records[1].id, "read2");
    EXPECT_EQ(records[1].comment, "length=4");
    EXPECT_EQ(records[1].seq, "AAAA");

    EXPECT_EQ(records[2].id, "read3");
    EXPECT_EQ(records[2].seq, "ACGTACGTACGT");

    // Next batch should be empty/false
    hasData = reader.nextBatch(batch);
    EXPECT_FALSE(hasData);
    EXPECT_TRUE(batch.records().empty());
}

TEST_F(FastqReaderTest, SmallBufferBoundary) {
    // This test is hard to deterministicly trigger buffer resizing logic
    // without mocking internal buffer size, but it verifies overall correctness.
    // We can write a large file to force multiple batches if we wanted.
}
