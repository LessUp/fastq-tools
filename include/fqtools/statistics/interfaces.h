/**
 * @file interfaces.h
 * @brief 统计接口：StatisticInterface 与 move-only Calculator
 */

#pragma once

#include "fqtools/io/fastq_io.h"
#include "fqtools/processing/processing_options.h"

#include <cstddef>
#include <memory>
#include <string>

namespace fq::statistics {

struct FqStatisticResult;

/// 统计计算接口，由 FqStatisticWorker 实现
class StatisticInterface {
public:
    using Batch = fq::io::FastqBatch;
    using Result = FqStatisticResult;

    virtual ~StatisticInterface() = default;

    virtual auto calculateStats(const Batch& batch) -> Result = 0;
};

using IStatistic = StatisticInterface;

/// 统计计算配置
struct StatisticOptions {
    std::string inputFastqPath;       ///< 输入 FASTQ 文件路径
    std::string outputStatPath;       ///< 输出统计结果路径（文本报告）
    std::string signatureReportPath;  ///< 可选 signature sidecar 路径（TSV）
    std::string jsonOutputPath;       ///< 可选 JSON 报告路径

    fq::processing::ProcessingOptions processing;

    size_t signatureKmerSize = 15;                ///< 头部 k-mer 长度
    size_t maxReportedSignatures = 20;            ///< sidecar 最大行数
    size_t duplicateEstimateSampleModulo = 1024;  ///< duplicate 估计采样模数
    int qualityEncoding = 33;                     ///< 质量编码偏移量
};

/// 统计计算器（move-only），通过 PIMPL 隐藏实现
class Calculator {
public:
    explicit Calculator(StatisticOptions options);
    ~Calculator();

    Calculator(const Calculator&) = delete;
    auto operator=(const Calculator&) -> Calculator& = delete;
    Calculator(Calculator&&) noexcept;
    auto operator=(Calculator&&) noexcept -> Calculator&;

    void run();

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/// 将已汇总的统计结果写成 TSV / JSON / signature sidecar（空路径跳过）
void writeStatisticsOutputs(const StatisticOptions& options, const FqStatisticResult& result);

}  // namespace fq::statistics
