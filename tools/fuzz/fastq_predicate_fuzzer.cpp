/**
 * @file fastq_predicate_fuzzer.cpp
 * @brief 真实 FASTQ 谓词模糊测试
 *
 * 直接调用 fq::processing 的 4 个真实 predicate：
 *   - MinQualityPredicate（平均质量，含 int64 累加 clamp 防下溢路径）
 *   - MinLengthPredicate
 *   - MaxLengthPredicate
 *   - MaxNRatioPredicate（除法，含空序列除零路径）
 *
 * 覆盖 ASCII < encoding 的负值累加、空 seq/qual、极端阈值等边界。
 *
 * 构建：cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ -B build-fuzz
 * 运行：./build-fuzz/fuzzers/fastq_predicate_fuzzer tools/fuzz/corpus/ -max_len=2048
 */

#include "fqtools/processing/predicates.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#include <fuzzer/FuzzedDataProvider.h>

namespace {

constexpr char kQualChars[] =
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
    "abcdefghijklmnopqrstuvwxyz{|}~\x00\x01\xff";

constexpr char kSeqChars[] = "ACGTNacgtn\x00\xff-z.";

auto makeRecord(const std::string& seqBuf, const std::string& qualBuf) -> fq::io::FastqRecord {
    fq::io::FastqRecord rec;
    rec.id = std::string_view("fuzz_id");
    rec.seq = std::string_view(seqBuf);
    rec.qual = std::string_view(qualBuf);
    rec.plus = std::string_view("+");
    return rec;
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > 32 * 1024) {
        return 0;
    }
    if (size < 4) {
        return 0;
    }

    FuzzedDataProvider dp(data, size);

    const auto seqLen = dp.ConsumeIntegralInRange<size_t>(0, 2048);
    const auto equalLen = dp.ConsumeBool();
    const auto qualLen = equalLen ? seqLen : dp.ConsumeIntegralInRange<size_t>(0, 2048);

    std::string seqBuf;
    seqBuf.reserve(seqLen);
    for (size_t i = 0; i < seqLen; ++i) {
        seqBuf.push_back(kSeqChars[dp.ConsumeIntegralInRange<size_t>(0, sizeof(kSeqChars) - 2)]);
    }
    std::string qualBuf;
    qualBuf.reserve(qualLen);
    for (size_t i = 0; i < qualLen; ++i) {
        qualBuf.push_back(kQualChars[dp.ConsumeIntegralInRange<size_t>(0, sizeof(kQualChars) - 2)]);
    }

    const auto rec = makeRecord(seqBuf, qualBuf);

    // MinQualityPredicate：threshold 覆盖负/0/中/超大，encoding 覆盖 0/33/64
    {
        const auto minQ = dp.ConsumeFloatingPointInRange<double>(-10.0, 200.0);
        const auto enc = dp.ConsumeIntegralInRange<int>(0, 96);
        fq::processing::MinQualityPredicate pred(minQ, enc);
        volatile bool pass = pred.evaluate(rec);
        (void)pass;
    }

    // MinLengthPredicate：0/中/超长阈值
    {
        const auto minLen = dp.ConsumeIntegralInRange<size_t>(0, 4096);
        fq::processing::MinLengthPredicate pred(minLen);
        volatile bool pass = pred.evaluate(rec);
        (void)pass;
    }

    // MaxLengthPredicate
    {
        const auto maxLen = dp.ConsumeIntegralInRange<size_t>(0, 4096);
        fq::processing::MaxLengthPredicate pred(maxLen);
        volatile bool pass = pred.evaluate(rec);
        (void)pass;
    }

    // MaxNRatioPredicate：0..1 边界
    {
        const auto maxRatio = dp.ConsumeFloatingPointInRange<double>(0.0, 1.0);
        fq::processing::MaxNRatioPredicate pred(maxRatio);
        volatile bool pass = pred.evaluate(rec);
        (void)pass;
    }

    return 0;
}
