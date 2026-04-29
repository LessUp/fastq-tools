/**
 * @file fq_statistic.h
 * @brief FASTQ 统计信息管理器类定义
 * @details 该文件定义了 FASTQ 统计信息管理器类，用于管理完整的 FASTQ 统计信息生成过程，
 *          利用 TBB 管道来提高性能
 *
 * @author FastQTools Team
 * @date 2024
 * @version 1.0
 *
 * @copyright Copyright (c) 2024 FastQTools
 * @license MIT License
 */

#pragma once

#include "fqtools/io/fastq_io.h"
#include "fqtools/statistics/statistic_calculator_interface.h"
#include "fqtools/statistics/statistic_interface.h"

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace fq::statistic {

// 统计常量（供 FqStatisticResult 和 FqStatisticWorker 共用）
constexpr int kMaxQual = 42;    ///< 最大质量分数值
constexpr int kMaxBaseNum = 5;  ///< 最大碱基数量

/**
 * @brief FASTQ 统计信息结果结构体
 * @details 存储 FASTQ 文件统计分析的结果数据，包括读取数量、长度分布、
 *          位置质量分数分布和位置碱基分布等信息
 */
struct FqStatisticResult {
    uint64_t readCount = 0;              ///< 总读取数量
    uint64_t totalBases = 0;             ///< 总碱基数
    uint32_t maxReadLength = 0;          ///< 最大读取长度
    uint64_t duplicateSampledReads = 0;  ///< 采样命中的 duplicate reads 数量

    /// 位置质量分数分布（扁平化一维布局：[pos * kMaxQual + qual]）
    std::vector<uint64_t> posQualityDist;
    /// 位置碱基分布（扁平化一维布局：[pos * kMaxBaseNum + base]）
    std::vector<uint64_t> posBaseDist;
    std::map<std::string, uint64_t> headKmerCounts;      ///< 头部 k-mer signature 计数
    std::map<uint64_t, uint64_t> sampledSequenceHashes;  ///< duplicate 采样哈希计数

    /// 确保分布数组能容纳 newLength 个位置
    void ensureCapacity(size_t newLength) {
        if (newLength > maxReadLength) {
            posQualityDist.resize(newLength * kMaxQual, 0);
            posBaseDist.resize(newLength * kMaxBaseNum, 0);
            maxReadLength = static_cast<uint32_t>(newLength);
        }
    }

    /// 按位置访问质量分布（返回指向该位置 kMaxQual 个槽位的指针）
    [[nodiscard]] auto qualityAt(size_t pos) -> uint64_t* {
        return posQualityDist.data() + pos * kMaxQual;
    }
    [[nodiscard]] auto qualityAt(size_t pos) const -> const uint64_t* {
        return posQualityDist.data() + pos * kMaxQual;
    }

    /// 按位置访问碱基分布（返回指向该位置 kMaxBaseNum 个槽位的指针）
    [[nodiscard]] auto baseAt(size_t pos) -> uint64_t* {
        return posBaseDist.data() + pos * kMaxBaseNum;
    }
    [[nodiscard]] auto baseAt(size_t pos) const -> const uint64_t* {
        return posBaseDist.data() + pos * kMaxBaseNum;
    }

    /**
     * @brief 重载 += 运算符，用于合并统计结果
     * @details 将另一个统计结果合并到当前结果中
     *
     * @param other 要合并的另一个统计结果
     * @return 合并后的统计结果引用
     */
    auto operator+=(const FqStatisticResult& other) -> FqStatisticResult&;
};

/**
 * @brief FASTQ 统计信息管理器
 * @details 该类使用 TBB 管道管理完整的 FASTQ 统计信息生成过程，
 *          是 StatisticCalculatorInterface 接口的具体实现
 *
 * @note 该类利用并行处理提高大文件的处理效率
 * @warning 处理过程中需要足够的内存空间
 */
class FastqStatisticCalculator : public StatisticCalculatorInterface {
public:
    /**
     * @brief 构造函数
     * @details 使用给定选项创建 FqStatistic 实例
     *
     * @param options 统计运行的配置选项
     * @pre options 必须包含有效的配置参数
     * @post 统计信息管理器被初始化并准备使用
     */
    explicit FastqStatisticCalculator(const StatisticOptions& options);

    /**
     * @brief 执行统计信息生成过程
     * @details 使用 TBB 并行管道执行完整的 FASTQ 统计信息生成过程
     * @post 统计结果被写入到指定的输出文件中
     */
    void run() override;

private:
    /**
     * @brief 将最终聚合的统计信息写入输出文件
     * @details 将统计结果以适当的格式写入到输出文件中
     *
     * @param result 要写入的最终结果
     * @pre result 必须包含有效的统计数据
     * @post 统计信息被写入到配置指定的输出文件中
     */
    void writeResult(const FqStatisticResult& result);
    void writeSignatureSidecar(const FqStatisticResult& result);

    StatisticOptions options_;  ///< 统计配置选项
};

}  // namespace fq::statistic
