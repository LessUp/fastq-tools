/**
 * @file fq_statistic.h
 * @brief FASTQ 统计结果结构 FqStatisticResult 及其合并语义
 */

#pragma once

#include "fqtools/io/fastq_io.h"
#include "fqtools/statistics/interfaces.h"

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace fq::statistics {

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
    std::map<std::string, uint64_t, std::less<>> headKmerCounts;  ///< 头部 k-mer signature 计数
    std::map<uint64_t, uint64_t> sampledSequenceHashes;           ///< duplicate 采样哈希计数

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
     */
    auto operator+=(const FqStatisticResult& other) -> FqStatisticResult&;
};

}  // namespace fq::statistics
