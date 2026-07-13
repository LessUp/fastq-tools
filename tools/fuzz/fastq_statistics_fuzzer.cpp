/**
 * @file fastq_statistics_fuzzer.cpp
 * @brief 真实 FASTQ 统计工作器模糊测试
 *
 * 直接调用 fq::statistic::FqStatisticWorker::calculateStats，
 * 覆盖累加/取模/除法、ensureCapacity 扩容、kBaseLut 越界索引、
 * ASCII < qualOffset_ 的 clamp 路径。
 *
 * 实现：构造 FastqBatch，将 seq/qual 追加进 batch.buffer()，
 * rec 的 string_view 指向 buffer 内偏移（与真实 nextBatch 同模式），
 * 保证 view 生命周期与 batch 绑定。
 *
 * 构建：cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ -B build-fuzz
 * 运行：./build-fuzz/fuzzers/fastq_statistics_fuzzer tools/fuzz/corpus/ -max_len=8192
 */

#include "fqtools/io/fastq_io.h"
#include "fqtools/statistics/interfaces.h"

#include "statistics/fq_statistic_worker.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <fuzzer/FuzzedDataProvider.h>

namespace {

constexpr char kQualChars[] =
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
    "abcdefghijklmnopqrstuvwxyz{|}~\x00\x01\xff";

constexpr char kSeqChars[] = "ACGTNacgtnRYMK\x00\xff-z.";

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > 128 * 1024) {
        return 0;
    }
    if (size < 4) {
        return 0;
    }

    FuzzedDataProvider dp(data, size);

    const auto qualOffset = dp.ConsumeIntegralInRange<int>(0, 96);
    const auto kmerSize = dp.ConsumeIntegralInRange<size_t>(0, 32);
    const auto sampleMod = dp.ConsumeIntegralInRange<size_t>(1, 4096);
    const auto recordCount = dp.ConsumeIntegralInRange<size_t>(0, 64);

    fq::statistic::FqStatisticWorker worker(qualOffset, kmerSize, sampleMod);

    // 阶段 1：生成所有 seq/qual 到局部 vector，累加总字节
    // 避免逐条 appendRecord 时 vector reallocation 使前一条 record 的 view 失效
    std::vector<std::string> seqs;
    std::vector<std::string> quals;
    seqs.reserve(recordCount);
    quals.reserve(recordCount);
    size_t totalBytes = 0;
    for (size_t r = 0; r < recordCount; ++r) {
        const auto seqLen = dp.ConsumeIntegralInRange<size_t>(0, 512);
        const auto equalLen = dp.ConsumeBool();
        const auto qualLen = equalLen ? seqLen : dp.ConsumeIntegralInRange<size_t>(0, 512);

        std::string seq;
        seq.reserve(seqLen);
        for (size_t i = 0; i < seqLen; ++i) {
            seq.push_back(kSeqChars[dp.ConsumeIntegralInRange<size_t>(0, sizeof(kSeqChars) - 2)]);
        }
        std::string qual;
        qual.reserve(qualLen);
        for (size_t i = 0; i < qualLen; ++i) {
            qual.push_back(kQualChars[dp.ConsumeIntegralInRange<size_t>(0, sizeof(kQualChars) - 2)]);
        }
        totalBytes += seq.size() + qual.size();
        seqs.push_back(std::move(seq));
        quals.push_back(std::move(qual));
    }

    // 阶段 2：一次性 reserve buffer，再连续 insert + 设 view（reserve 后不再 realloc）
    fq::io::FastqBatch batch(64 * 1024);
    auto& buf = batch.buffer();
    buf.reserve(totalBytes);
    auto& records = batch.records();
    records.reserve(recordCount);
    for (size_t r = 0; r < recordCount; ++r) {
        const auto seqOff = buf.size();
        buf.insert(buf.end(), seqs[r].begin(), seqs[r].end());
        const auto qualOff = buf.size();
        buf.insert(buf.end(), quals[r].begin(), quals[r].end());

        fq::io::FastqRecord rec;
        rec.id = std::string_view("fuzz_id");
        rec.seq = std::string_view(buf.data() + seqOff, seqs[r].size());
        rec.qual = std::string_view(buf.data() + qualOff, quals[r].size());
        rec.plus = std::string_view("+");
        records.push_back(rec);
    }

    auto result = worker.calculateStats(batch);

    // 访问结果触发越界读
    volatile uint64_t readCount = result.readCount;
    volatile uint64_t totalBases = result.totalBases;
    (void)readCount;
    (void)totalBases;
    if (result.maxReadLength > 0) {
        // 访问最后一个位置的质量/碱基分布槽
        const auto last = static_cast<size_t>(result.maxReadLength) - 1;
        volatile uint64_t q = result.qualityAt(last)[0];
        volatile uint64_t b = result.baseAt(last)[0];
        (void)q;
        (void)b;
    }

    return 0;
}
