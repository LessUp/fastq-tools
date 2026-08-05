/**
 * @file statistics_writer.h
 * @brief 统计结果格式化输出器
 */

#pragma once

#include "fqtools/statistics/interfaces.h"

#include <cstdint>
#include <ostream>
#include <string>

namespace fq::statistics {

struct StatisticsWriterOptions {
    std::string inputFastqPath;                   ///< 输入文件路径（用于报告头）
    int qualityEncoding = 33;                     ///< 质量编码偏移
    size_t duplicateEstimateSampleModulo = 1024;  ///< 重复估计采样模数
    std::string signatureReportPath;              ///< 签名报告路径（空则不生成）
    size_t maxReportedSignatures = 20;            ///< 最大报告签名数
};

/// 将 FqStatisticResult 格式化为文本报告，接受 std::ostream& 便于测试
class StatisticsWriter {
public:
    explicit StatisticsWriter(StatisticsWriterOptions options);
    ~StatisticsWriter() = default;

    StatisticsWriter(const StatisticsWriter&) = delete;
    StatisticsWriter& operator=(const StatisticsWriter&) = delete;
    StatisticsWriter(StatisticsWriter&&) noexcept = default;
    StatisticsWriter& operator=(StatisticsWriter&&) noexcept = default;

    void write(std::ostream& os, const FqStatisticResult& result);
    void writeSignature(std::ostream& os, const FqStatisticResult& result);

private:
    StatisticsWriterOptions options_;
};

}  // namespace fq::statistics
