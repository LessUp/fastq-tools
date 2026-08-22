#include "fqtools/processing/predicates/min_quality_predicate.h"

#include <cstdint>

#include <fmt/format.h>

namespace fq::processing {

// --- MinQualityPredicate ---

MinQualityPredicate::MinQualityPredicate(double minQuality, int qualityEncoding)
    : minQuality_(minQuality), qualityEncoding_(qualityEncoding) {}

auto MinQualityPredicate::evaluate(const fq::io::FastqRecord& read) const -> bool {
    totalEvaluated_.increment();
    if (read.qual.empty()) {
        return false;
    }

    double avgQual = calculateAverageQuality(read.qual);

    if (avgQual >= minQuality_) {
        passedCount_.increment();
        return true;
    }
    return false;
}

auto MinQualityPredicate::calculateAverageQuality(std::string_view qualityString) const -> double {
    if (qualityString.empty()) {
        return 0.0;
    }

    // 用 int64_t 累加并 clamp 负值到 0，避免 uint64_t 下溢成巨大正值
    // 导致坏数据（ASCII < encoding）反而通过质控
    int64_t sumQual = 0;
    for (char q : qualityString) {
        // 显式按 int8_t 解释：非法字节（>=128）在 x86/ARM 上一致视为负质量
        const int qVal = static_cast<int>(static_cast<std::int8_t>(q)) - qualityEncoding_;
        sumQual += (qVal > 0) ? qVal : 0;
    }
    return static_cast<double>(sumQual) / static_cast<double>(qualityString.size());
}

auto MinQualityPredicate::getName() const -> std::string {
    return "MinQualityPredicate";
}

auto MinQualityPredicate::getDescription() const -> std::string {
    return fmt::format("Filters reads with average quality < {:.2f}", minQuality_);
}

auto MinQualityPredicate::getStatistics() const -> std::string {
    size_t total = totalEvaluated_.load();
    size_t passed = passedCount_.load();
    double percent =
        total > 0 ? (100.0 * static_cast<double>(passed) / static_cast<double>(total)) : 0.0;
    return fmt::format("MinQuality: Passed {}/{} ({:.2f}%)", passed, total, percent);
}

// --- MinLengthPredicate ---

MinLengthPredicate::MinLengthPredicate(size_t minLength) : minLength_(minLength) {}

auto MinLengthPredicate::evaluate(const fq::io::FastqRecord& read) const -> bool {
    totalEvaluated_.increment();
    if (read.seq.size() >= minLength_) {
        passedCount_.increment();
        return true;
    }
    return false;
}

auto MinLengthPredicate::getName() const -> std::string {
    return "MinLengthPredicate";
}
auto MinLengthPredicate::getDescription() const -> std::string {
    return fmt::format("Filters reads shorter than {} bp", minLength_);
}
auto MinLengthPredicate::getStatistics() const -> std::string {
    return fmt::format("MinLength: Passed {}/{}", passedCount_.load(), totalEvaluated_.load());
}

// --- MaxLengthPredicate ---

MaxLengthPredicate::MaxLengthPredicate(size_t maxLength) : maxLength_(maxLength) {}

auto MaxLengthPredicate::evaluate(const fq::io::FastqRecord& read) const -> bool {
    totalEvaluated_.increment();
    if (read.seq.size() <= maxLength_) {
        passedCount_.increment();
        return true;
    }
    return false;
}

auto MaxLengthPredicate::getName() const -> std::string {
    return "MaxLengthPredicate";
}
auto MaxLengthPredicate::getDescription() const -> std::string {
    return fmt::format("Filters reads longer than {} bp", maxLength_);
}
auto MaxLengthPredicate::getStatistics() const -> std::string {
    return fmt::format("MaxLength: Passed {}/{}", passedCount_.load(), totalEvaluated_.load());
}

// --- MaxNRatioPredicate ---

MaxNRatioPredicate::MaxNRatioPredicate(double maxNRatio) : maxNRatio_(maxNRatio) {}

auto MaxNRatioPredicate::evaluate(const fq::io::FastqRecord& read) const -> bool {
    totalEvaluated_.increment();
    if (calculateNRatio(read.seq) <= maxNRatio_) {
        passedCount_.increment();
        return true;
    }
    return false;
}

auto MaxNRatioPredicate::calculateNRatio(std::string_view sequence) const -> double {
    if (sequence.empty()) {
        return 0.0;
    }
    size_t nCount = 0;
    for (char c : sequence) {
        if (c == 'N' || c == 'n') {
            nCount++;
        }
    }
    return static_cast<double>(nCount) / static_cast<double>(sequence.size());
}

auto MaxNRatioPredicate::getName() const -> std::string {
    return "MaxNRatioPredicate";
}
auto MaxNRatioPredicate::getDescription() const -> std::string {
    return fmt::format("Filters reads with N ratio > {:.2f}", maxNRatio_);
}
auto MaxNRatioPredicate::getStatistics() const -> std::string {
    return fmt::format("MaxNRatio: Passed {}/{}", passedCount_.load(), totalEvaluated_.load());
}

}  // namespace fq::processing
