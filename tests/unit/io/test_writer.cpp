#include "fqtools/io/fastq_writer.h"
#include "fqtools/io/fastq_io.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <zlib.h>

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
    } // Flush and close in dtor

    // Since it's gzipped, we use zlib to read it back or just check if it's non-empty
    EXPECT_TRUE(std::filesystem::exists(tmpFile_));
    EXPECT_GT(std::filesystem::file_size(tmpFile_), 0);

    // Briefly verify it's a valid gzip file by opening with gzopen
    gzFile file = gzopen(tmpFile_.c_str(), "rb");
    EXPECT_NE(file, nullptr);
    
    char buffer[1024];
    int len = gzread(file, buffer, sizeof(buffer));
    EXPECT_GT(len, 0);
    std::string content(buffer, len);
    EXPECT_NE(content.find("@read1"), std::string::npos);
    EXPECT_NE(content.find("ACGT"), std::string::npos);
    EXPECT_NE(content.find("@read2 desc"), std::string::npos);
    
    gzclose(file);
}

} // namespace fq::io
