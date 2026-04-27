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
 * @version 1.0
 * @copyright (c) 2023 LessUp. All rights reserved.
 */

#pragma once

#include "fqtools/processing/execution_backend.h"
#include "fqtools/processing/memory_resource_policy.h"

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
    std::string inputFastqPath;  ///< 输入 FASTQ 文件路径
    std::string outputStatPath;  ///< 输出统计结果路径（JSON）
    uint32_t batchSize = 50000;  ///< 每批次处理的记录数

    uint32_t threadCount = 4;                      ///< 并行线程数
    fq::processing::ExecutionBackend executionBackend =
        fq::processing::ExecutionBackend::OneTbb;  ///< 执行后端
    fq::processing::MemoryResourcePolicy memoryResourcePolicy =
        fq::processing::MemoryResourcePolicy::ObjectPool;  ///< 内存资源策略
    bool allocationTelemetryEnabled = false;               ///< 是否启用内存遥测
    size_t readChunkBytes = 1 * 1024 * 1024;       ///< 读取块大小（字节）
    size_t zlibBufferBytes = 128 * 1024;           ///< zlib 缓冲区大小（字节）
    size_t batchCapacityBytes = 4 * 1024 * 1024;  ///< 批次容量（字节）
    size_t memoryLimitBytes = 0;                  ///< 内存限制（0 表示无限制）
    size_t maxInFlightBatches = 0;                ///< 最大并行批次（0 表示自动）
    int qualityEncoding = 33;                     ///< 质量编码偏移量（Phred+33）
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
