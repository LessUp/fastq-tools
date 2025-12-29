#pragma once

#include "fqtools/io/fastq_io.h"

#include <atomic>
#include <string>
#include <vector>

#include "fqtools/processing/read_mutator_interface.h"

namespace fq::processing {

class QualityTrimmer : public ReadMutatorInterface {
public:
    enum class TrimMode { Both, FivePrime, ThreePrime };

    QualityTrimmer(double qualityThreshold,
                   size_t minLength = 1,
                   TrimMode mode = TrimMode::Both,
                   int qualityEncoding = 33);

    void process(fq::io::FastqRecord& read) override;

    auto getName() const -> std::string;
    auto getDescription() const -> std::string;
    void reset();

private:
    double qualityThreshold_;
    size_t minLength_;
    TrimMode trimMode_;
    int qualityEncoding_;

    std::atomic<size_t> totalProcessed_{0};
    std::atomic<size_t> trimmedCount_{0};
    std::atomic<size_t> totalBasesRemoved_{0};

    auto trimFivePrime(std::string_view sequence, std::string_view quality) const -> size_t;
    auto trimThreePrime(std::string_view sequence, std::string_view quality) const -> size_t;
    auto isHighQuality(char qualityChar) const -> bool;
};

class LengthTrimmer : public ReadMutatorInterface {
public:
    enum class TrimStrategy { FixedLength, MaxLength, FromStart, FromEnd };

    LengthTrimmer(size_t targetLength, TrimStrategy strategy = TrimStrategy::FixedLength);

    void process(fq::io::FastqRecord& read) override;

    auto getName() const -> std::string;
    auto getDescription() const -> std::string;
    void reset();

private:
    size_t targetLength_;
    TrimStrategy strategy_;

    std::atomic<size_t> totalProcessed_{0};
    std::atomic<size_t> trimmedCount_{0};
    std::atomic<size_t> totalBasesRemoved_{0};
};

class AdapterTrimmer : public ReadMutatorInterface {
public:
    AdapterTrimmer(const std::vector<std::string>& adapterSequences,
                   size_t minOverlap = 3,
                   size_t maxMismatches = 1);

    void process(fq::io::FastqRecord& read) override;

    auto getName() const -> std::string;
    auto getDescription() const -> std::string;
    void reset();

private:
    std::vector<std::string> adapters_;
    size_t minOverlap_;
    size_t maxMismatches_;

    std::atomic<size_t> totalProcessed_{0};
    std::atomic<size_t> adapterFound_{0};
    std::atomic<size_t> totalBasesRemoved_{0};

    auto findAdapter(std::string_view sequence, std::string_view adapter) const -> size_t;
    auto countMismatches(std::string_view seq1, std::string_view seq2) const -> size_t;
};

}  // namespace fq::processing
