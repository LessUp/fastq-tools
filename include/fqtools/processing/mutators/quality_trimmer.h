#pragma once

#include "fqtools/processing/interfaces.h"

#include <string>
#include <vector>

namespace fq::processing {

class QualityTrimmer : public ReadMutatorInterface {
public:
    enum class TrimMode { Both, FivePrime, ThreePrime };

    QualityTrimmer(double qualityThreshold,
                   size_t minLength = 1,
                   TrimMode mode = TrimMode::Both,
                   int qualityEncoding = 33);

    void process(fq::io::FastqRecord& read) override;

    [[nodiscard]] auto getName() const -> std::string;
    [[nodiscard]] auto getDescription() const -> std::string;

private:
    double qualityThreshold_;
    size_t minLength_;
    TrimMode trimMode_;
    int qualityEncoding_;

    [[nodiscard]] auto trimFivePrime(std::string_view sequence, std::string_view quality) const
        -> size_t;
    [[nodiscard]] auto trimThreePrime(std::string_view sequence, std::string_view quality) const
        -> size_t;
    [[nodiscard]] auto isHighQuality(char qualityChar) const -> bool;
};

class LengthTrimmer : public ReadMutatorInterface {
public:
    enum class TrimStrategy { FixedLength, MaxLength, FromStart, FromEnd };

    LengthTrimmer(size_t targetLength, TrimStrategy strategy = TrimStrategy::FixedLength);

    void process(fq::io::FastqRecord& read) override;

    [[nodiscard]] auto getName() const -> std::string;
    [[nodiscard]] auto getDescription() const -> std::string;

private:
    size_t targetLength_;
    TrimStrategy strategy_;
};

class AdapterTrimmer : public ReadMutatorInterface {
public:
    AdapterTrimmer(const std::vector<std::string>& adapterSequences,
                   size_t minOverlap = 3,
                   size_t maxMismatches = 1);

    void process(fq::io::FastqRecord& read) override;

    [[nodiscard]] auto getName() const -> std::string;
    [[nodiscard]] auto getDescription() const -> std::string;

private:
    std::vector<std::string> adapters_;
    size_t minOverlap_;
    size_t maxMismatches_;

    [[nodiscard]] auto findAdapter(std::string_view sequence, std::string_view adapter) const
        -> size_t;
    [[nodiscard]] auto countMismatches(std::string_view seq1, std::string_view seq2) const
        -> size_t;
};

class PolyTailTrimmer : public ReadMutatorInterface {
public:
    enum class TailKind { PolyG, PolyX };

    PolyTailTrimmer(TailKind kind, size_t minRunLength = 10);

    void process(fq::io::FastqRecord& read) override;

    [[nodiscard]] auto getName() const -> std::string;
    [[nodiscard]] auto getDescription() const -> std::string;

private:
    TailKind kind_;
    size_t minRunLength_;

    [[nodiscard]] auto trimPosition(std::string_view sequence) const -> size_t;
    [[nodiscard]] static auto normalizeBase(char base) -> char;
};

}  // namespace fq::processing
