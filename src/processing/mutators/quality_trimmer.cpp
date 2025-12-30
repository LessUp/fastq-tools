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
    totalProcessed_++;

    if (read.empty())
        return;

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
        totalBasesRemoved_ += originalLen;
        trimmedCount_++;  // Considered trimmed completely
    } else {
        // Apply trim
        if (newLen < originalLen) {
            read.seq = read.seq.substr(start, newLen);
            read.qual = read.qual.substr(start, newLen);
            totalBasesRemoved_ += (originalLen - newLen);
            trimmedCount_++;
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
void QualityTrimmer::reset() {
    totalProcessed_ = 0;
    trimmedCount_ = 0;
    totalBasesRemoved_ = 0;
}

// --- LengthTrimmer ---

LengthTrimmer::LengthTrimmer(size_t targetLength, TrimStrategy strategy)
    : targetLength_(targetLength), strategy_(strategy) {}

void LengthTrimmer::process(fq::io::FastqRecord& read) {
    totalProcessed_++;
    size_t len = read.seq.size();

    if (len <= targetLength_ && strategy_ != TrimStrategy::FixedLength) {
        // Nothing to do if shorter (unless FixedLength padding logic? usually not implemented)
        return;
    }

    size_t newLen = len;
    size_t start = 0;

    switch (strategy_) {
        case TrimStrategy::FixedLength:
        case TrimStrategy::MaxLength:
        case TrimStrategy::FromEnd:  // Keep first N bases
            if (len > targetLength_) {
                newLen = targetLength_;
            }
            break;
        case TrimStrategy::FromStart:  // Trim first N bases? No, name implies "Keep from start"?
            // Usually "HeadCrop" removes first N.
            // But "FromStart" strategy here likely means "Trim N bases from start".
            // Wait, target_length usually means "Resulting length".
            // If strategy is "FromStart", maybe it means "Keep the end, remove start so length is
            // N"? Or "Remove N bases from start"? Let's assume "Trim to length N by removing from
            // start" (Keep last N)
            if (len > targetLength_) {
                start = len - targetLength_;
                newLen = targetLength_;
            }
            break;
    }

    if (newLen < len) {
        read.seq = read.seq.substr(start, newLen);
        read.qual = read.qual.substr(start, newLen);
        totalBasesRemoved_ += (len - newLen);
        trimmedCount_++;
    }
}

auto LengthTrimmer::getName() const -> std::string {
    return "LengthTrimmer";
}
auto LengthTrimmer::getDescription() const -> std::string {
    return fmt::format("Trims reads to length {}", targetLength_);
}
void LengthTrimmer::reset() {
    totalProcessed_ = 0;
    trimmedCount_ = 0;
    totalBasesRemoved_ = 0;
}

// --- AdapterTrimmer ---

AdapterTrimmer::AdapterTrimmer(const std::vector<std::string>& adapterSequences,
                               size_t minOverlap,
                               size_t maxMismatches)
    : adapters_(adapterSequences), minOverlap_(minOverlap), maxMismatches_(maxMismatches) {}

void AdapterTrimmer::process(fq::io::FastqRecord& read) {
    totalProcessed_++;
    if (read.empty())
        return;

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
        size_t originalLen = read.seq.size();
        // Trim everything from bestPos
        read.seq = read.seq.substr(0, bestPos);
        read.qual = read.qual.substr(0, bestPos);

        totalBasesRemoved_ += (originalLen - bestPos);
        adapterFound_++;
    }
}

auto AdapterTrimmer::findAdapter(std::string_view sequence, std::string_view adapter) const
    -> size_t {
    // Very basic implementation: search for exact match or partial overlap at 3' end
    // Ideally use semi-global alignment or specialized library (e.g. ksw2)
    // For simplicity here: check suffix of seq vs prefix of adapter

    // 1. Check if adapter is inside sequence
    size_t pos = sequence.find(adapter);
    if (pos != std::string_view::npos)
        return pos;

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

    for (size_t i = startCheck; i <= seqLen - minOverlap_; ++i) {
        // Compare sequence[i...] with adapter[0...]
        size_t overlapLen = seqLen - i;
        // If overlapLen > adLen, logic error above, but loop handles it

        size_t mismatches =
            countMismatches(sequence, adapter.substr(0, overlapLen));  // View creation is cheap
        if (mismatches <= maxMismatches_) {
            return i;
        }
    }

    return std::string::npos;
}

auto AdapterTrimmer::countMismatches(std::string_view seq1, std::string_view seq2) const
    -> size_t {
    size_t len = std::min(seq1.size(), seq2.size());
    // Adjust inputs to be same length if passed different views?
    // The usage above ensures we compare suffix of seq1 vs prefix of seq2 of same length.
    // But safe to iterate min length.

    size_t mis = 0;
    for (size_t i = 0; i < len; ++i) {
        if (seq1[i] != seq2[i]) {  // Case sensitive? Usually yes for adapters
            mis++;
            if (mis > maxMismatches_)
                return mis;
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
void AdapterTrimmer::reset() {
    totalProcessed_ = 0;
    adapterFound_ = 0;
    totalBasesRemoved_ = 0;
}

}  // namespace fq::processing
