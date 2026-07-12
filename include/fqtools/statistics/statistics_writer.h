/**
 * @file statistics_writer.h
 * @brief 统计结果输出器
 * @details 将 FqStatisticResult 格式化输出到流，支持文本报告和签名报告。
 *
 * @author FastQTools Team
 * @date 2026
 * @version 2.0
 * @copyright Copyright (c) 2026 FastQTools
 * @license MIT License
 */

#pragma once

#include "fqtools/statistics/interfaces.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <ostream>
#include <string>

namespace fq::statistic {

/**
 * @brief 统计报告输出配置
 */
struct StatisticsWriterOptions {
    std::string inputFastqPath;                   ///< 输入文件路径（用于报告头）
    int qualityEncoding = 33;                     ///< 质量编码偏移
    size_t duplicateEstimateSampleModulo = 1024;  ///< 重复估计采样模数
    std::string signatureReportPath;              ///< 签名报告路径（空则不生成）
    size_t maxReportedSignatures = 20;            ///< 最大报告签名数
};

/**
 * @brief 统计结果格式化输出器
 * @details 将 FqStatisticResult 格式化为文本报告输出到流。
 *          支持输出到文件或内存流，便于测试。
 *
 * 设计原则：
 * - 接受 std::ostream& 而非文件路径，便于测试
 * - 格式化逻辑集中，便于维护
 * - 不依赖 I/O 实现细节
 */
class StatisticsWriter {
public:
    explicit StatisticsWriter(StatisticsWriterOptions options);
    ~StatisticsWriter() = default;

    // 禁止拷贝
    StatisticsWriter(const StatisticsWriter&) = delete;
    StatisticsWriter& operator=(const StatisticsWriter&) = delete;

    // 允许移动
    StatisticsWriter(StatisticsWriter&&) noexcept = default;
    StatisticsWriter& operator=(StatisticsWriter&&) noexcept = default;

    /**
     * @brief 将统计结果写入输出流
     * @param os 输出流（可以是文件流、字符串流等）
     * @param result 统计结果
     */
    void write(std::ostream& os, const FqStatisticResult& result);

    /**
     * @brief 将签名报告写入输出流
     * @param os 输出流
     * @param result 统计结果
     */
    void writeSignature(std::ostream& os, const FqStatisticResult& result);

private:
    StatisticsWriterOptions options_;
};

}  // namespace fq::statistic
