/**
 * @file statistic_interface.h
 * @brief 统计计算接口定义
 * @details 定义用于 FASTQ 数据统计分析的接口。
 *
 * 统计接口（StatisticInterface）用于实现各种统计分析，
 * 例如：碱基组成分布、质量分数分布、GC 含量等。
 *
 * @author LessUp
 * @date 2023-10-05
 * @version 1.0
 * @copyright (c) 2023 LessUp. All rights reserved.
 */

#pragma once

#include "fqtools/io/fastq_io.h"

namespace fq::statistic {

// 前向声明
struct FqStatisticResult;

/**
 * @brief 统计计算接口
 * @details 抽象基类，用于定义统计分析操作。
 *
 * 实现自定义统计：
 * @code
 * class MyStatistic : public StatisticInterface {
 * public:
 *     auto calculateStats(const Batch& batch) -> Result override {
 *         Result result;
 *         // 计算统计信息
 *         return result;
 *     }
 * };
 * @endcode
 *
 * @note 所有统计实现必须重写 calculateStats() 方法
 */
class StatisticInterface {
public:
    using Batch = fq::io::FastqBatch;   ///< 批次类型别名
    using Result = FqStatisticResult;   ///< 结果类型别名

    virtual ~StatisticInterface() = default;

    /**
     * @brief 计算批次统计信息
     * @param batch 待统计的 FASTQ 数据批次
     * @return 统计结果
     */
    virtual auto calculateStats(const Batch& batch) -> Result = 0;
};

/// @brief StatisticInterface 的类型别名（兼容旧代码）
using IStatistic = StatisticInterface;

}  // namespace fq::statistic
