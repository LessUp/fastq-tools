#include "fqtools/processing/mutators/quality_trimmer.h"

#include <algorithm>

#include <fmt/format.h>

#ifdef __AVX2__
#include <immintrin.h>
#endif

namespace fq::processing {

// --- QualityTrimmer ---

QualityTrimmer::QualityTrimmer(double qualityThreshold,
                               size_t minLength,
                               TrimMode mode,
                               int qualityEncoding)
    : qualityThreshold_(qualityThreshold),
      minLength_(minLength),
      trimMode_(mode),
      qualityEncoding_(qualityEncoding) {}

void QualityTrimmer::process(fq::io::FastqRecord& read) {
    if (read.empty()) {
        return;
    }

    size_t originalLen = read.seq.size();
    size_t start = 0;
    size_t end = originalLen;

    // Trim 5'
    if (trimMode_ == TrimMode::Both || trimMode_ == TrimMode::FivePrime) {
        start = trimFivePrime(read.seq, read.qual);
    }

    // Trim 3'
    if (trimMode_ == TrimMode::Both || trimMode_ == TrimMode::ThreePrime) {
        // Only trim 3' if we still have bases left
        if (start < end) {
            std::string_view currentSeq = read.seq.substr(start, end - start);
            std::string_view currentQual = read.qual.substr(start, end - start);

            size_t newLen = trimThreePrime(currentSeq, currentQual);
            end = start + newLen;
        }
    }

    size_t newLen = (end > start) ? (end - start) : 0;

    // Check min length
    if (newLen < minLength_) {
        // Filter out (make empty)
        read.seq = {};
        read.qual = {};
    } else {
        // Apply trim
        if (newLen < originalLen) {
            read.seq = read.seq.substr(start, newLen);
            read.qual = read.qual.substr(start, newLen);
        }
    }
}

auto QualityTrimmer::trimFivePrime(std::string_view sequence, std::string_view quality) const
    -> size_t {
    size_t len = std::min(sequence.size(), quality.size());
    size_t i = 0;

#ifdef __AVX2__
    // Align? No usually just loadu.
    // Quality is char, encoded.
    // Threshold check: qual[i] - encoding >= threshold
    // => qual[i] >= threshold + encoding
    // Let target = threshold + encoding
    int target = static_cast<int>(qualityThreshold_) + qualityEncoding_;
    // If target > 127 or so, be careful with signed comparisons.
    // char is usually signed. -128 to 127.
    // Quality scores are usually 33..70+.
    // _mm256_cmpgt_epi8 does signed comparison.
    // If target > 127, we might have issues if char is signed.
    // Standard fastq qual is printable ASCII (33-126).
    // So all positive in signed char.

    __m256i vTarget = _mm256_set1_epi8(static_cast<char>(target - 1));
    // We want to find FIRST char where q >= target.
    // Equivalent to q > target - 1.
    // So we invoke cmpgt(q, target - 1).

    for (; i + 32 <= len; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(quality.data() + i));
        __m256i result = _mm256_cmpgt_epi8(chunk, vTarget);
        int mask = _mm256_movemask_epi8(result);
        if (mask != 0) {
            // Found a high quality base
            return i + __builtin_ctz(mask);
        }
    }
    // Handle remaining... falls through to scalar
#endif

    for (; i < len; ++i) {
        if (isHighQuality(quality[i])) {
            return i;
        }
    }
    return len;  // Trim all
}

auto QualityTrimmer::trimThreePrime(std::string_view sequence, std::string_view quality) const
    -> size_t {
    size_t len = std::min(sequence.size(), quality.size());
    // Scan from end
    size_t i = len;
    while (i > 0) {
        if (isHighQuality(quality[i - 1])) {
            return i;
        }
        i--;
    }
    return 0;  // Trim all
}

auto QualityTrimmer::isHighQuality(char qualityChar) const -> bool {
    int q = static_cast<int>(qualityChar) - qualityEncoding_;
    return q >= qualityThreshold_;
}

auto QualityTrimmer::getName() const -> std::string {
    return "QualityTrimmer";
}
auto QualityTrimmer::getDescription() const -> std::string {
    return fmt::format("Trims bases with quality < {:.2f}", qualityThreshold_);
}
// --- LengthTrimmer ---

LengthTrimmer::LengthTrimmer(size_t targetLength, TrimStrategy strategy)
    : targetLength_(targetLength), strategy_(strategy) {}

void LengthTrimmer::process(fq::io::FastqRecord& read) {
    const size_t len = read.seq.size();
    if (len <= targetLength_) {
        return;
    }
    // MaxLength: 保留前 N 个碱基；FromStart: 保留末尾 N 个碱基
    const size_t start = (strategy_ == TrimStrategy::FromStart) ? (len - targetLength_) : 0;
    read.seq = read.seq.substr(start, targetLength_);
    read.qual = read.qual.substr(start, targetLength_);
}

auto LengthTrimmer::getName() const -> std::string {
    return "LengthTrimmer";
}
auto LengthTrimmer::getDescription() const -> std::string {
    return fmt::format("Trims reads to length {}", targetLength_);
}
// --- AdapterTrimmer ---

AdapterTrimmer::AdapterTrimmer(const std::vector<std::string>& adapterSequences,
                               size_t minOverlap,
                               size_t maxMismatches)
    : adapters_(adapterSequences), minOverlap_(minOverlap), maxMismatches_(maxMismatches) {}

void AdapterTrimmer::process(fq::io::FastqRecord& read) {
    if (read.empty()) {
        return;
    }

    // Simple implementation: check 3' end for adapter
    // If found, trim from that position

    size_t bestPos = std::string::npos;  // Position to trim from (smallest index)

    for (const auto& adapter : adapters_) {
        size_t pos = findAdapter(read.seq, adapter);
        if (pos != std::string::npos) {
            if (bestPos == std::string::npos || pos < bestPos) {
                bestPos = pos;
            }
        }
    }

    if (bestPos != std::string::npos) {
        // Trim everything from bestPos
        read.seq = read.seq.substr(0, bestPos);
        read.qual = read.qual.substr(0, bestPos);
    }
}

auto AdapterTrimmer::findAdapter(std::string_view sequence, std::string_view adapter) const
    -> size_t {
    // Very basic implementation: search for exact match or partial overlap at 3' end
    // Ideally use semi-global alignment or specialized library (e.g. ksw2)
    // For simplicity here: check suffix of seq vs prefix of adapter

    // 1. Check if adapter is inside sequence
    if (adapter.empty() || sequence.size() < minOverlap_ || adapter.size() < minOverlap_) {
        return std::string::npos;
    }

    size_t pos = sequence.find(adapter);
    if (pos != std::string_view::npos) {
        return pos;
    }

    // 2. Check 3' overlap
    // Adapter starts within sequence and continues
    // sequence: ...XXXXYYYY
    // adapter:     XXXXYYYYZZZZ...

    size_t seqLen = sequence.size();
    size_t adLen = adapter.size();

    // We look for overlap of at least minOverlap_
    // Start checking from pos = seqLen - adLen (or 0)
    // We shift adapter along the sequence 3' end

    size_t startCheck = (seqLen > adLen) ? (seqLen - adLen) : 0;

    const size_t lastStart = seqLen - minOverlap_;
    for (size_t i = startCheck; i <= lastStart; ++i) {
        // Compare sequence[i...] with adapter[0...]
        size_t overlapLen = seqLen - i;
        // If overlapLen > adLen, logic error above, but loop handles it

        size_t mismatches = countMismatches(sequence.substr(i), adapter.substr(0, overlapLen));
        if (mismatches <= maxMismatches_) {
            return i;
        }
    }

    return std::string::npos;
}

auto AdapterTrimmer::countMismatches(std::string_view seq1, std::string_view seq2) const -> size_t {
    size_t len = std::min(seq1.size(), seq2.size());
    // Adjust inputs to be same length if passed different views?
    // The usage above ensures we compare suffix of seq1 vs prefix of seq2 of same length.
    // But safe to iterate min length.

    size_t mis = 0;
    for (size_t i = 0; i < len; ++i) {
        if (seq1[i] != seq2[i]) {  // Case sensitive? Usually yes for adapters
            mis++;
            if (mis > maxMismatches_) {
                return mis;
            }
        }
    }
    return mis;
}

auto AdapterTrimmer::getName() const -> std::string {
    return "AdapterTrimmer";
}
auto AdapterTrimmer::getDescription() const -> std::string {
    return "Trims adapter sequences";
}
// --- PolyTailTrimmer ---

PolyTailTrimmer::PolyTailTrimmer(TailKind kind, size_t minRunLength)
    : kind_(kind), minRunLength_(std::max<size_t>(1, minRunLength)) {}

void PolyTailTrimmer::process(fq::io::FastqRecord& read) {
    if (read.empty()) {
        return;
    }

    const size_t trimPos = trimPosition(read.seq);
    if (trimPos >= read.seq.size()) {
        return;
    }

    read.seq = read.seq.substr(0, trimPos);
    read.qual = read.qual.substr(0, trimPos);
}

auto PolyTailTrimmer::getName() const -> std::string {
    return kind_ == TailKind::PolyG ? "PolyGTailTrimmer" : "PolyXTailTrimmer";
}

auto PolyTailTrimmer::getDescription() const -> std::string {
    return kind_ == TailKind::PolyG
        ? fmt::format("Trims polyG tails with run >= {}", minRunLength_)
        : fmt::format("Trims low-complexity polyX tails with run >= {}", minRunLength_);
}

auto PolyTailTrimmer::trimPosition(std::string_view sequence) const -> size_t {
    if (sequence.size() < minRunLength_) {
        return sequence.size();
    }

    char tailBase = '\0';
    size_t runLength = 0;
    for (size_t i = sequence.size(); i > 0; --i) {
        const char normalized = normalizeBase(sequence[i - 1]);
        if (normalized == '\0') {
            break;
        }

        if (tailBase == '\0') {
            tailBase = normalized;
            runLength = 1;
        } else if (normalized == tailBase) {
            ++runLength;
        } else {
            break;
        }
    }

    if (runLength < minRunLength_) {
        return sequence.size();
    }

    if (kind_ == TailKind::PolyG && tailBase != 'G') {
        return sequence.size();
    }

    return sequence.size() - runLength;
}

auto PolyTailTrimmer::normalizeBase(char base) -> char {
    switch (base) {
        case 'A':
        case 'a':
            return 'A';
        case 'C':
        case 'c':
            return 'C';
        case 'G':
        case 'g':
            return 'G';
        case 'T':
        case 't':
            return 'T';
        default:
            return '\0';
    }
}

}  // namespace fq::processing
