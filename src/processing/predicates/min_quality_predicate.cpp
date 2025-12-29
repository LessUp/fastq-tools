#include "fqtools/processing/predicates/min_quality_predicate.h"

#include <numeric>

#include <fmt/format.h>

namespace fq::processing {

// --- MinQualityPredicate ---

MinQualityPredicate::MinQualityPredicate(double minQuality, int qualityEncoding)
    : minQuality_(minQuality), qualityEncoding_(qualityEncoding) {}

auto MinQualityPredicate::evaluate(const fq::io::FastqRecord& read) const -> bool {
    totalEvaluated_.fetch_add(1, std::memory_order_relaxed);
    if (read.qual.empty())
        return false;

    double avgQual = calculateAverageQuality(read.qual);

    if (avgQual >= minQuality_) {
        passedCount_.fetch_add(1, std::memory_order_relaxed);
        return true;
    }
    return false;
}

auto MinQualityPredicate::calculateAverageQuality(std::string_view qualityString) const
    -> double {
    if (qualityString.empty())
        return 0.0;

    uint64_t sumQual = 0;
    for (char q : qualityString) {
        sumQual += (static_cast<int>(q) - qualityEncoding_);
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
    double percent = total > 0 ? (100.0 * passed / total) : 0.0;
    return fmt::format("MinQuality: Passed {}/{} ({:.2f}%)", passed, total, percent);
}

// --- MinLengthPredicate ---

MinLengthPredicate::MinLengthPredicate(size_t minLength) : minLength_(minLength) {}

auto MinLengthPredicate::evaluate(const fq::io::FastqRecord& read) const -> bool {
    totalEvaluated_.fetch_add(1, std::memory_order_relaxed);
    if (read.seq.size() >= minLength_) {
        passedCount_.fetch_add(1, std::memory_order_relaxed);
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
    totalEvaluated_.fetch_add(1, std::memory_order_relaxed);
    if (read.seq.size() <= maxLength_) {
        passedCount_.fetch_add(1, std::memory_order_relaxed);
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
    totalEvaluated_.fetch_add(1, std::memory_order_relaxed);
    if (calculateNRatio(read.seq) <= maxNRatio_) {
        passedCount_.fetch_add(1, std::memory_order_relaxed);
        return true;
    }
    return false;
}

auto MaxNRatioPredicate::calculateNRatio(std::string_view sequence) const -> double {
    if (sequence.empty())
        return 0.0;
    size_t nCount = 0;
    for (char c : sequence) {
        if (c == 'N' || c == 'n')
            nCount++;
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
