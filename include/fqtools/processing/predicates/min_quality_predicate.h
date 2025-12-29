#pragma once

#include "fqtools/io/fastq_io.h"

#include <atomic>
#include <string>

#include "fqtools/processing/read_predicate_interface.h"

namespace fq::processing {

class MinQualityPredicate : public ReadPredicateInterface {
public:
    explicit MinQualityPredicate(double minQuality, int qualityEncoding = 33);
    auto evaluate(const fq::io::FastqRecord& read) const -> bool override;

    auto getName() const -> std::string;
    auto getDescription() const -> std::string;
    auto getStatistics() const -> std::string;

private:
    double minQuality_;
    int qualityEncoding_;
    mutable std::atomic<size_t> totalEvaluated_{0};
    mutable std::atomic<size_t> passedCount_{0};

    // Helper to calculate average quality from string_view
    auto calculateAverageQuality(std::string_view qualityString) const -> double;
};

class MinLengthPredicate : public ReadPredicateInterface {
public:
    explicit MinLengthPredicate(size_t minLength);
    auto evaluate(const fq::io::FastqRecord& read) const -> bool override;

    auto getName() const -> std::string;
    auto getDescription() const -> std::string;
    auto getStatistics() const -> std::string;

private:
    size_t minLength_;
    mutable std::atomic<size_t> totalEvaluated_{0};
    mutable std::atomic<size_t> passedCount_{0};
};

class MaxLengthPredicate : public ReadPredicateInterface {
public:
    explicit MaxLengthPredicate(size_t maxLength);
    auto evaluate(const fq::io::FastqRecord& read) const -> bool override;

    auto getName() const -> std::string;
    auto getDescription() const -> std::string;
    auto getStatistics() const -> std::string;

private:
    size_t maxLength_;
    mutable std::atomic<size_t> totalEvaluated_{0};
    mutable std::atomic<size_t> passedCount_{0};
};

class MaxNRatioPredicate : public ReadPredicateInterface {
public:
    explicit MaxNRatioPredicate(double maxNRatio);
    auto evaluate(const fq::io::FastqRecord& read) const -> bool override;

    auto getName() const -> std::string;
    auto getDescription() const -> std::string;
    auto getStatistics() const -> std::string;

private:
    double maxNRatio_;
    mutable std::atomic<size_t> totalEvaluated_{0};
    mutable std::atomic<size_t> passedCount_{0};

    auto calculateNRatio(std::string_view sequence) const -> double;
};

}  // namespace fq::processing
