#include "fqtools/io/fastq_io.h"
#include "fqtools/io/fastq_writer.h"

#include <filesystem>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

namespace fq::io {

class FastqWriterTest : public ::testing::Test {
protected:
    void SetUp() override {
        tmpFile_ = "test_writer_output.fastq";
    }

    void TearDown() override {
        if (std::filesystem::exists(tmpFile_)) {
            std::filesystem::remove(tmpFile_);
        }
    }

    std::string tmpFile_;
};

TEST_F(FastqWriterTest, WriteBasic) {
    {
        FastqWriter writer(tmpFile_);
        EXPECT_TRUE(writer.isOpen());

        FastqRecord rec;
        rec.id = "read1";
        rec.seq = "ACGT";
        rec.qual = "IIII";
        writer.write(rec);

        FastqRecord rec2;
        rec2.id = "read2";
        rec2.comment = "desc";
        rec2.seq = "AAAA";
        rec2.qual = "JJJJ";
        writer.write(rec2);
    }

    ASSERT_TRUE(std::filesystem::exists(tmpFile_));
    EXPECT_GT(std::filesystem::file_size(tmpFile_), 0);

    std::ifstream in(tmpFile_);
    ASSERT_TRUE(in.is_open());
    const std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("@read1\nACGT\n+\nIIII\n"), std::string::npos);
    EXPECT_NE(content.find("@read2 desc\nAAAA\n+\nJJJJ\n"), std::string::npos);
}

TEST_F(FastqWriterTest, PreservesCustomPlusLineWhenWriting) {
    {
        FastqWriter writer(tmpFile_);
        ASSERT_TRUE(writer.isOpen());

        FastqRecord rec;
        rec.id = "read1";
        rec.comment = "desc";
        rec.seq = "ACGT";
        rec.plus = "+read1 desc";
        rec.qual = "IIII";
        writer.write(rec);
    }

    std::ifstream in(tmpFile_);
    ASSERT_TRUE(in.is_open());
    const std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    EXPECT_NE(content.find("@read1 desc\nACGT\n+read1 desc\nIIII\n"), std::string::npos);
}

}  // namespace fq::io
