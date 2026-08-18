/**
 * @file fq_statistic_result.cpp
 * @brief FqStatisticResult 合并语义
 */

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "statistics/fq_statistic.h"

namespace fq::statistics {

auto FqStatisticResult::operator+=(const FqStatisticResult& other) -> FqStatisticResult& {
    this->readCount += other.readCount;
    this->totalBases += other.totalBases;
    this->duplicateSampledReads += other.duplicateSampledReads;

    // 确保 this 能容纳 other 的最大长度
    if (other.maxReadLength > this->maxReadLength) {
        ensureCapacity(other.maxReadLength);
    }

    // 向量化友好的合并：使用 __restrict__ 指针消除别名分析障碍，
    // 让编译器 -O3 -ftree-vectorize 能生成 AVX2 SIMD 指令
    {
        const size_t n = std::min(posQualityDist.size(), other.posQualityDist.size());
        uint64_t* __restrict__ dst = posQualityDist.data();
        const uint64_t* __restrict__ src = other.posQualityDist.data();
        for (size_t i = 0; i < n; ++i) {
            dst[i] += src[i];
        }
    }

    {
        const size_t n = std::min(posBaseDist.size(), other.posBaseDist.size());
        uint64_t* __restrict__ dst = posBaseDist.data();
        const uint64_t* __restrict__ src = other.posBaseDist.data();
        for (size_t i = 0; i < n; ++i) {
            dst[i] += src[i];
        }
    }

    for (const auto& [hashValue, count] : other.sampledSequenceHashes) {
        auto& current = sampledSequenceHashes[hashValue];
        if (current > 0 && count > 0) {
            ++duplicateSampledReads;
        }
        current += count;
    }

    // 合并每批的有界 sketch 到全局有界 sketch：worker 侧每批限 64 个 key，
    // 但高多样性数据下批间几乎不重复，无界合并会随批数线性膨胀
    // （10 万批 → 数百万 map 节点）。允许临时超界一倍，超限后按计数保留 top-K——
    // 报告只取 top-N（N << K），剪枝不影响结果。
    constexpr size_t kMaxHeadKmerEntries = 4096;
    for (const auto& [kmer, count] : other.headKmerCounts) {
        headKmerCounts[kmer] += count;
    }
    if (headKmerCounts.size() > kMaxHeadKmerEntries * 2) {
        std::vector<std::pair<std::string, uint64_t>> entries(headKmerCounts.begin(),
                                                              headKmerCounts.end());
        std::partial_sort(entries.begin(),
                          entries.begin() + kMaxHeadKmerEntries,
                          entries.end(),
                          [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });
        headKmerCounts.clear();
        for (size_t i = 0; i < kMaxHeadKmerEntries; ++i) {
            headKmerCounts.emplace(std::move(entries[i].first), entries[i].second);
        }
    }

    return *this;
}

}  // namespace fq::statistics
