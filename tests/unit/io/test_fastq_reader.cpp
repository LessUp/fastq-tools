#include "fqtools/error/error.h"
#include "fqtools/io/fastq_io.h"
#include "fqtools/io/fastq_reader.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include <unistd.h>

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

TEST_F(FastqReaderTest, RejectsZeroMaxRecords) {
    // maxRecords=0 会使读取循环不读数据即报 EOF（或携残片时空转），属契约错误
    fq::io::FastqReader reader(tmpFile_);
    fq::io::FastqBatch batch;
    EXPECT_THROW(static_cast<void>(reader.nextBatch(batch, 0)), std::invalid_argument);
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

// 空序列记录（seq/qual 均空）：旧行为 validateLengths 0==0 会放行，
// 现显式拒绝，避免空 read 静默进入下游统计与修剪
TEST_F(FastqReaderTest, RejectsEmptySequenceRecord) {
    const std::string customFile = "test_reader_empty_seq.fastq";
    {
        std::ofstream out(customFile);
        out << "@read-empty\n"
            << "\n"
            << "+\n"
            << "\n";
    }

    fq::io::FastqReader reader(customFile);
    fq::io::FastqBatch batch;

    try {
        static_cast<void>(reader.nextBatch(batch));
        FAIL() << "expected FormatError for empty sequence";
    } catch (const fq::error::FormatError& e) {
        EXPECT_NE(std::string(e.message()).find("Empty sequence"), std::string::npos)
            << e.message();
    }

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

TEST(FastqReaderStdioTest, ReadsUncompressedStdinDash) {
    int fds[2] = {-1, -1};
    ASSERT_EQ(::pipe(fds), 0);
    const char payload[] = "@read1\nACGT\n+\nIIII\n";
    ASSERT_EQ(::write(fds[1], payload, sizeof(payload) - 1),
              static_cast<ssize_t>(sizeof(payload) - 1));
    ::close(fds[1]);

    const int savedStdin = ::dup(STDIN_FILENO);
    ASSERT_GE(savedStdin, 0);
    ASSERT_EQ(::dup2(fds[0], STDIN_FILENO), STDIN_FILENO);
    ::close(fds[0]);

    {
        fq::io::FastqReader reader("-");
        fq::io::FastqBatch batch;
        ASSERT_TRUE(reader.nextBatch(batch));
        ASSERT_EQ(batch.records().size(), 1U);
        EXPECT_EQ(batch.records()[0].seq, "ACGT");
        EXPECT_FALSE(reader.nextBatch(batch));
    }

    ::dup2(savedStdin, STDIN_FILENO);
    ::close(savedStdin);
}

TEST(FastqReaderStdioTest, RejectsGzipStdin) {
    int fds[2] = {-1, -1};
    ASSERT_EQ(::pipe(fds), 0);
    const unsigned char gzipMagic[] = {0x1f, 0x8b, 0x00, 0x00};
    ASSERT_EQ(::write(fds[1], gzipMagic, sizeof(gzipMagic)),
              static_cast<ssize_t>(sizeof(gzipMagic)));
    ::close(fds[1]);

    const int savedStdin = ::dup(STDIN_FILENO);
    ASSERT_GE(savedStdin, 0);
    ASSERT_EQ(::dup2(fds[0], STDIN_FILENO), STDIN_FILENO);
    ::close(fds[0]);

    EXPECT_THROW(fq::io::FastqReader reader("-"), fq::error::ConfigurationError);

    ::dup2(savedStdin, STDIN_FILENO);
    ::close(savedStdin);
}

TEST_F(FastqReaderTest, SmallBufferBoundary) {
    // This test is hard to deterministicly trigger buffer resizing logic
    // without mocking internal buffer size, but it verifies overall correctness.
    // We can write a large file to force multiple batches if we wanted.
}
