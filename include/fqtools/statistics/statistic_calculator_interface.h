/**
 * @file statistic_calculator_interface.h
 * @brief 统计计算器接口定义
 * @details 定义高层统计计算任务接口，解耦 CLI 层与具体实现。
 *
 * 该接口提供完整的统计计算流程抽象，包括：
 * - 文件读取
 * - 并行处理
 * - 结果输出
 *
 * @author LessUp
 * @date 2023-10-05
 * @version 2.0
 * @copyright (c) 2023 LessUp. All rights reserved.
 */

#pragma once

#include "fqtools/processing/processing_options.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace fq::statistic {

/**
 * @brief 统计计算配置选项
 * @details 定义统计计算任务的所有配置参数。
 */
struct StatisticOptions {
    /// @name 文件路径
    /// @{
    std::string inputFastqPath;                   ///< 输入 FASTQ 文件路径
    std::string outputStatPath;                   ///< 输出统计结果路径（文本报告）
    std::string signatureReportPath;              ///< 可选的 signature sidecar 路径（TSV）
    /// @}

    /// @name 处理选项
    /// @{
    fq::processing::ProcessingOptions processing; ///< 处理管道选项
    /// @}

    /// @name 统计特定参数
    /// @{
    size_t signatureKmerSize = 15;                ///< signature 使用的头部 k-mer 长度
    size_t maxReportedSignatures = 20;            ///< sidecar 中最多输出的 signature 数
    size_t duplicateEstimateSampleModulo = 1024;  ///< duplicate 估计的哈希采样模数
    int qualityEncoding = 33;                     ///< 质量编码偏移量（Phred+33）
    /// @}

    // 兼容性字段（从 processing 中获取）
    [[nodiscard]] auto batchSize() const -> uint32_t {
        return static_cast<uint32_t>(processing.batchSize);
    }
    [[nodiscard]] auto threadCount() const -> uint32_t {
        return static_cast<uint32_t>(processing.threadCount);
    }
};

/**
 * @brief 统计计算器接口
 * @details 高层统计计算任务抽象，封装完整的计算流程。
 *
 * 使用工厂函数创建实例：
 * @code
 * auto calculator = createStatisticCalculator(options);
 * calculator->run();
 * @endcode
 */
class StatisticCalculatorInterface {
public:
    virtual ~StatisticCalculatorInterface() = default;

    /**
     * @brief 执行统计计算
     * @details 执行完整的统计生成流程，包括：
     *   - 打开输入文件
     *   - 并行读取和处理
     *   - 合并统计结果
     *   - 输出到文件
     */
    virtual void run() = 0;
};

/**
 * @brief 创建统计计算器实例
 * @param options 统计计算配置
 * @return 统计计算器实例（unique_ptr）
 */
auto createStatisticCalculator(const StatisticOptions& options)
    -> std::unique_ptr<StatisticCalculatorInterface>;

}  // namespace fq::statistic
