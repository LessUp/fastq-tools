#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <stdexcept>
#include <string>

#include <fcntl.h>
#include <unistd.h>

#include "statistics/fq_statistic.h"
#include "statistics/statistics_report.h"
#include <fqtools/statistics/statistics_writer.h>
#include <gtest/gtest.h>

namespace fq::statistics {

// 捕获 stderr 内容（logging 直接写 fd 2），用于断言 warn 行为。
// 回归：此前硬编码 /tmp 固定路径，目录不存在时 open 失败仍继续执行，
// content() 对空 FILE* 调用 fgets 导致段错误。现改为系统临时目录下的
// 唯一文件 + 创建失败快速抛错，content() 直接从已持有的 fd 读取。
class StderrCapture {
public:
    StderrCapture() {
        std::fflush(stderr);
        path_ = (std::filesystem::temp_directory_path() /
                 ("fqtools_stderr_" + std::to_string(::getpid()) + "_" + std::to_string(counter++) +
                  ".txt"))
                    .string();
        captured_ = ::open(path_.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0600);
        if (captured_ < 0) {
            throw std::runtime_error("StderrCapture: failed to create temp capture file");
        }
        saved_ = ::dup(STDERR_FILENO);
        ::dup2(captured_, STDERR_FILENO);
    }
    ~StderrCapture() {
        std::fflush(stderr);
        ::dup2(saved_, STDERR_FILENO);
        ::close(saved_);
        ::close(captured_);
        ::unlink(path_.c_str());
    }

    auto content() -> std::string {
        std::fflush(stderr);
        std::string out;
        char buffer[512];
        off_t offset = 0;
        while (true) {
            const auto bytesRead = ::pread(captured_, buffer, sizeof(buffer), offset);
            if (bytesRead <= 0) {
                break;
            }
            out.append(buffer, static_cast<size_t>(bytesRead));
            offset += bytesRead;
        }
        return out;
    }

private:
    inline static int counter = 0;
    std::string path_;
    int captured_ = -1;
    int saved_ = -1;
};


TEST(StatisticsReportTest, BuildsSummaryPositionAndSignatureLines) {
    FqStatisticResult result;
    result.readCount = 2;
    result.totalBases = 4;
    result.duplicateSampledReads = 1;
    result.ensureCapacity(2);

    result.baseAt(0)[0] = 1;
    result.baseAt(0)[1] = 1;
    result.baseAt(1)[2] = 1;
    result.baseAt(1)[3] = 1;

    result.qualityAt(0)[20] = 2;
    result.qualityAt(1)[30] = 2;

    result.headKmerCounts["ACGT"] = 2;

    StatisticsWriterOptions options;
    options.inputFastqPath = "/tmp/input.fastq";
    options.qualityEncoding = 33;
    options.duplicateEstimateSampleModulo = 2;
    options.maxReportedSignatures = 5;

    const auto report = buildStatisticsReport(result, options);

    EXPECT_EQ(report.summaryLines.front(), "#Name\tinput.fastq");
    EXPECT_NE(
        std::find(report.summaryLines.begin(), report.summaryLines.end(), "#DuplicateEstimate\t2"),
        report.summaryLines.end());
    EXPECT_EQ(report.positionLines.front(), "#Pos\tA\tC\tG\tT\tN\tAvgQual\tErrRate");
    EXPECT_EQ(report.signatureLines.front(), "metric\tkey\tcount");
    EXPECT_NE(
        std::find(report.signatureLines.begin(), report.signatureLines.end(), "head_kmer\tACGT\t2"),
        report.signatureLines.end());
}

TEST(StatisticsReportTest, UsesPerPositionQualityStrideForErrorRate) {
    FqStatisticResult result;
    result.readCount = 201;
    result.totalBases = 201;
    result.ensureCapacity(2);

    result.baseAt(0)[0] = 1;
    result.baseAt(1)[0] = 200;
    result.qualityAt(0)[20] = 1;
    result.qualityAt(1)[0] = 200;

    StatisticsWriterOptions options;
    options.inputFastqPath = "/tmp/input.fastq";
    options.qualityEncoding = 33;
    options.duplicateEstimateSampleModulo = 1;
    options.maxReportedSignatures = 5;

    const auto report = buildStatisticsReport(result, options);

    ASSERT_GE(report.positionLines.size(), 3U);
    EXPECT_EQ(report.positionLines[1], "1\t1\t0\t0\t0\t0\t20.00\t1.00");
}

// RFC 8259：U+0000..U+001F 控制字符必须转义，否则产出非法 JSON
TEST(StatisticsReportTest, JsonEscapesControlCharactersInName) {
    FqStatisticResult result;
    result.readCount = 1;
    result.totalBases = 1;
    result.ensureCapacity(1);

    StatisticsWriterOptions options;
    options.inputFastqPath = std::string("/tmp/ba\x01r\b.fq");
    const auto json = formatStatisticsJson(result, options);

    // \x01 与 \b 均须以 \uXXXX 形式出现，原始控制字节不得进入输出
    EXPECT_NE(json.find("\\u0001"), std::string::npos);
    EXPECT_NE(json.find("\\u0008"), std::string::npos);
    EXPECT_EQ(json.find('\x01'), std::string::npos);
    EXPECT_EQ(json.find('\b'), std::string::npos);
}

TEST(StatisticsReportTest, FormatsJsonWithSameMetricsAsTsv) {
    FqStatisticResult result;
    result.readCount = 2;
    result.totalBases = 4;
    result.duplicateSampledReads = 1;
    result.ensureCapacity(2);

    result.baseAt(0)[0] = 1;
    result.baseAt(0)[1] = 1;
    result.baseAt(1)[2] = 1;
    result.baseAt(1)[3] = 1;

    result.qualityAt(0)[20] = 2;
    result.qualityAt(1)[30] = 2;

    StatisticsWriterOptions options;
    options.inputFastqPath = "/tmp/input.fastq";
    options.qualityEncoding = 33;
    options.duplicateEstimateSampleModulo = 2;

    const auto json = formatStatisticsJson(result, options);

    EXPECT_NE(json.find("\"name\": \"input.fastq\""), std::string::npos);
    EXPECT_NE(json.find("\"phred_qual\": 33"), std::string::npos);
    EXPECT_NE(json.find("\"read_num\": 2"), std::string::npos);
    EXPECT_NE(json.find("\"duplicate_estimate\": 2"), std::string::npos);
    EXPECT_NE(json.find("\"max_read_length\": 2"), std::string::npos);
    EXPECT_NE(json.find("\"base_count\": 4"), std::string::npos);
    EXPECT_NE(json.find("\"q20\""), std::string::npos);
    EXPECT_NE(json.find("\"count\": 4"), std::string::npos);
    EXPECT_NE(json.find("\"q30\""), std::string::npos);
    EXPECT_NE(json.find("\"GC\""), std::string::npos);
    EXPECT_NE(json.find("\"pos\": 1"), std::string::npos);
    EXPECT_NE(json.find("\"avg_qual\": 20"), std::string::npos);
    EXPECT_EQ(json.front(), '{');
    EXPECT_EQ(json.back(), '\n');
}

TEST(StatisticsReportTest, HandlesZeroTotalBasesWithoutInfOrNanSummary) {
    FqStatisticResult result;
    result.readCount = 2;
    result.totalBases = 0;

    StatisticsWriterOptions options;
    options.inputFastqPath = "/tmp/input.fastq";
    options.qualityEncoding = 33;
    options.duplicateEstimateSampleModulo = 2;
    options.maxReportedSignatures = 5;

    const auto report = buildStatisticsReport(result, options);

    EXPECT_NE(
        std::find(report.summaryLines.begin(), report.summaryLines.end(), "#Q20(>=20)\t0\t0.00%"),
        report.summaryLines.end());
    EXPECT_NE(std::find(report.summaryLines.begin(), report.summaryLines.end(), "#GC\t0\t0.00%"),
              report.summaryLines.end());
}

// 空数据时三种输出格式都应给出一致的无数据警告，
// 而不是 TSV/signature 静默产出空文件、JSON 静默产出全零结构
TEST(StatisticsWriterTest, WarnsOnEmptyResultForAllFormats) {
    FqStatisticResult empty;
    StatisticsWriterOptions options;
    options.inputFastqPath = "/tmp/input.fastq";
    StatisticsWriter writer(options);

    StderrCapture capture;
    std::ostringstream tsvOut;
    writer.write(tsvOut, empty);
    writer.writeSignature(tsvOut, empty);
    writer.writeJson(tsvOut, empty);
    const auto logged = capture.content();

    // 三种格式各警告一次
    size_t count = 0;
    for (auto pos = logged.find("No reads found"); pos != std::string::npos;
         pos = logged.find("No reads found", pos + 1)) {
        ++count;
    }
    EXPECT_EQ(count, 3u) << "logged: " << logged;
}

// 字符设备目标（/dev/null 等）不支持"同目录临时文件 + rename"的原子发布，
// 必须直接顺序写入；writeStatisticsOutputs 对特殊文件目标不得抛错
TEST(StatisticsWriterTest, WritesToCharacterDeviceWithoutAtomicTemporary) {
    FqStatisticResult result;
    result.readCount = 1;

    StatisticOptions options;
    options.inputFastqPath = "input.fastq";
    options.outputStatPath = "/dev/null";

    EXPECT_NO_THROW(writeStatisticsOutputs(options, result));
}

}  // namespace fq::statistics
