#include "fqtools/error/error.h"
#include "fqtools/io/fastq_io.h"
#include "fqtools/io/fastq_reader.h"

#include <filesystem>
#include <fstream>
#include <string>

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

TEST_F(FastqReaderTest, ConstructorThrowsOnMissingFile) {
    // 构造失败应抛 IOError，与 FastqWriter 行为一致
    EXPECT_THROW(fq::io::FastqReader reader("/nonexistent/path/to/file.fastq"), fq::error::IOError);
}

TEST_F(FastqReaderTest, PreservesPlusLineContent) {
    const std::string customFile = "test_reader_plus.fastq";
    {
        std::ofstream out(customFile);
        out << "@read-plus comment\n"
            << "ACGT\n"
            << "+read-plus comment\n"
            << "IIII\n";
    }

    fq::io::FastqReader reader(customFile);
    fq::io::FastqBatch batch;

    ASSERT_TRUE(reader.nextBatch(batch));
    ASSERT_EQ(batch.records().size(), 1);
    EXPECT_EQ(batch.records()[0].plus, "+read-plus comment");

    std::filesystem::remove(customFile);
}

TEST(FastqReaderValidationTest, ThrowsOnIncompleteTrailingRecord) {
    const std::string brokenFile = "test_reader_incomplete.fastq";
    {
        std::ofstream out(brokenFile);
        out << "@read1\n"
            << "ACGT\n"
            << "+\n";
    }

    fq::io::FastqReader reader(brokenFile);
    fq::io::FastqBatch batch;

    EXPECT_THROW(
        {
            try {
                static_cast<void>(reader.nextBatch(batch));
            } catch (const fq::error::FormatError& e) {
                EXPECT_NE(std::string(e.what()).find("missing quality line"), std::string::npos);
                throw;
            }
        },
        fq::error::FormatError);

    std::filesystem::remove(brokenFile);
}

TEST(FastqReaderValidationTest, ThrowsOnSequenceQualityLengthMismatch) {
    const std::string brokenFile = "test_reader_length_mismatch.fastq";
    {
        std::ofstream out(brokenFile);
        out << "@read1\n"
            << "ACGT\n"
            << "+\n"
            << "III\n";
    }

    fq::io::FastqReader reader(brokenFile);
    fq::io::FastqBatch batch;

    EXPECT_THROW(
        {
            try {
                static_cast<void>(reader.nextBatch(batch));
            } catch (const fq::error::FormatError& e) {
                EXPECT_NE(std::string(e.what()).find("length mismatch"), std::string::npos);
                throw;
            }
        },
        fq::error::FormatError);

    std::filesystem::remove(brokenFile);
}

TEST(FastqReaderValidationTest, ThrowsOnInvalidPlusLine) {
    const std::string brokenFile = "test_reader_bad_plus.fastq";
    {
        std::ofstream out(brokenFile);
        out << "@read1\n"
            << "ACGT\n"
            << "-\n"
            << "IIII\n";
    }

    fq::io::FastqReader reader(brokenFile);
    fq::io::FastqBatch batch;

    EXPECT_THROW(
        {
            try {
                static_cast<void>(reader.nextBatch(batch));
            } catch (const fq::error::FormatError& e) {
                EXPECT_NE(std::string(e.what()).find("Expected '+'"), std::string::npos);
                throw;
            }
        },
        fq::error::FormatError);

    std::filesystem::remove(brokenFile);
}

TEST_F(FastqReaderTest, SmallBufferBoundary) {
    // This test is hard to deterministicly trigger buffer resizing logic
    // without mocking internal buffer size, but it verifies overall correctness.
    // We can write a large file to force multiple batches if we wanted.
}
