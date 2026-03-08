#include "statistics/fq_statistic_worker.h"

#include "fqtools/io/fastq_io.h"

#include <array>
#include <cstdint>

#include "statistics/fq_statistic.h"

namespace fq::statistic {

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

}  // namespace

FqStatisticWorker::FqStatisticWorker(int qualOffset) : qualOffset_(qualOffset) {}

auto FqStatisticWorker::calculateStats(const Batch& batch) -> IStatistic::Result {
    FqStatisticResult result;
    if (batch.empty()) {
        return result;
    }

    for (const auto& read : batch) {
        result.readCount++;
        const size_t len = read.seq.size();
        result.totalBases += len;

        // 确保扁平化数组容量足够（仅在遇到更长 read 时 resize）
        result.ensureCapacity(len);

        const char* seqPtr = read.seq.data();
        const char* qualPtr = read.qual.data();

        for (size_t i = 0; i < len; ++i) {
            // 质量分数统计（clamp 到 [0, kMaxQual)）
            int qVal = static_cast<int>(qualPtr[i]) - qualOffset_;
            if (qVal < 0) {
                qVal = 0;
            }
            if (qVal >= kMaxQual) {
                qVal = kMaxQual - 1;
            }
            result.qualityAt(i)[qVal]++;

            // 碱基统计（查找表，无分支）
            result.baseAt(i)[kBaseLut[static_cast<unsigned char>(seqPtr[i])]]++;
        }
    }

    return result;
}

}  // namespace fq::statistic
