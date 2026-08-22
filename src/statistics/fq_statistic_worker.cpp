#include "statistics/fq_statistic_worker.h"

#include "fqtools/io/fastq_io.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>

#include "statistics/fq_statistic.h"

namespace fq::statistics {

namespace {

/// 碱基字符 → 索引查找表（编译期构造，消除 switch/case 分支预测开销）
/// A/a=0, C/c=1, G/g=2, T/t=3, 其余=4(N)
constexpr auto buildBaseLut() -> std::array<uint8_t, 256> {
    std::array<uint8_t, 256> lut{};
    for (auto& v : lut) {
        v = 4;  // 默认为 N
    }
    lut[static_cast<unsigned char>('A')] = 0;
    lut[static_cast<unsigned char>('a')] = 0;
    lut[static_cast<unsigned char>('C')] = 1;
    lut[static_cast<unsigned char>('c')] = 1;
    lut[static_cast<unsigned char>('G')] = 2;
    lut[static_cast<unsigned char>('g')] = 2;
    lut[static_cast<unsigned char>('T')] = 3;
    lut[static_cast<unsigned char>('t')] = 3;
    return lut;
}

constexpr auto kBaseLut = buildBaseLut();

void updateBoundedHeadKmers(std::map<std::string, uint64_t, std::less<>>& counts,
                            std::string_view key,
                            size_t maxEntries) {
    if (maxEntries == 0) {
        return;
    }

    auto it = counts.find(key);
    if (it != counts.end()) {
        ++it->second;
        return;
    }

    if (counts.size() < maxEntries) {
        counts.emplace(std::string(key), 1);
        return;
    }

    auto minIt =
        std::min_element(counts.begin(), counts.end(), [](const auto& lhs, const auto& rhs) {
            if (lhs.second != rhs.second) {
                return lhs.second < rhs.second;
            }
            return lhs.first < rhs.first;
        });
    if (minIt == counts.end()) {
        return;
    }

    // Space-Saving：新 key 替换计数最小者并继承其计数+1，
    // 保证表内计数 >= 真实出现次数 - 已淘汰次数，top-K 近似有意义；
    // 不能只削减最小者而丢弃新 key（会低估现有条目且新 kmer 永远无法积累）
    const uint64_t inheritedCount = minIt->second;
    counts.erase(minIt);
    counts.emplace(std::string(key), inheritedCount + 1);
}

}  // namespace

FqStatisticWorker::FqStatisticWorker(int qualOffset,
                                     size_t signatureKmerSize,
                                     size_t duplicateEstimateSampleModulo)
    : qualOffset_(qualOffset),
      signatureKmerSize_(std::max<size_t>(1, signatureKmerSize)),
      duplicateEstimateSampleModulo_(std::max<size_t>(1, duplicateEstimateSampleModulo)) {}

auto FqStatisticWorker::calculateStats(const Batch& batch) -> IStatistic::Result {
    FqStatisticResult result;
    if (batch.empty()) {
        return result;
    }

    for (const auto& read : batch) {
        result.readCount++;
        const size_t len = read.seq.size();
        result.totalBases += len;
        const size_t hashValue = std::hash<std::string_view>{}(read.seq);
        if ((hashValue % duplicateEstimateSampleModulo_) == 0) {
            auto& sampleCount = result.sampledSequenceHashes[hashValue];
            if (sampleCount > 0) {
                ++result.duplicateSampledReads;
            }
            ++sampleCount;
        }

        if (len >= signatureKmerSize_) {
            updateBoundedHeadKmers(
                result.headKmerCounts, read.seq.substr(0, signatureKmerSize_), 64);
        } else if (len > 0) {
            updateBoundedHeadKmers(result.headKmerCounts, read.seq, 64);
        }

        // 确保扁平化数组容量足够（仅在遇到更长 read 时 resize）
        result.ensureCapacity(len);

        const char* seqPtr = read.seq.data();

        // 碱基统计（查找表，无分支）
        for (size_t i = 0; i < len; ++i) {
            result.baseAt(i)[kBaseLut[static_cast<unsigned char>(seqPtr[i])]]++;
        }

        // 质量分数统计（clamp 到 [0, kMaxQual)）。
        // 上界取 min(seq, qual)：内置 reader 保证两者等长，但 calculateStats 是公共接口，
        // 自定义 IStatistic/IReader 可注入不等长记录，防御性限界避免堆越界读。
        const size_t qualLen = std::min(len, read.qual.size());
        for (size_t i = 0; i < qualLen; ++i) {
            // char 符号性平台差异：显式按 int8_t 解释，使非法字节（>=128）在
            // x86/ARM 上一致视为负质量（与 AVX2 有符号比较语义一致）
            int qVal = static_cast<int>(static_cast<std::int8_t>(read.qual[i])) - qualOffset_;
            if (qVal < 0) {
                qVal = 0;
            }
            if (qVal >= kMaxQual) {
                qVal = kMaxQual - 1;
            }
            result.qualityAt(i)[qVal]++;
        }
    }

    return result;
}

}  // namespace fq::statistics
