/**
 * @file fastq_record_fuzzer.cpp
 * @brief Fuzz testing for FASTQ record validation and manipulation
 *
 * This fuzzer tests record validation, quality score conversion,
 * and other record manipulation functions.
 *
 * Build: cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ ..
 * Run:   ./fuzzers/fastq_record_fuzzer corpus/ -max_len=1024
 */

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <numeric>

namespace {

/**
 * @brief Convert Phred+33 quality score to numeric value
 */
int qualityToPhred(char c) {
    return static_cast<int>(c) - 33;
}

/**
 * @brief Convert numeric Phred score to ASCII character
 */
char phredToQuality(int phred) {
    if (phred < 0) phred = 0;
    if (phred > 93) phred = 93;
    return static_cast<char>(phred + 33);
}

/**
 * @brief Calculate average quality score
 */
double averageQuality(const std::string& qual) {
    if (qual.empty()) return 0.0;
    
    double sum = 0.0;
    for (char c : qual) {
        sum += qualityToPhred(c);
    }
    return sum / static_cast<double>(qual.size());
}

/**
 * @brief Count bases by type
 */
struct BaseCounts {
    size_t a = 0, c = 0, g = 0, t = 0, n = 0, other = 0;
};

BaseCounts countBases(const std::string& seq) {
    BaseCounts counts;
    for (char c : seq) {
        switch (c) {
            case 'A': case 'a': counts.a++; break;
            case 'C': case 'c': counts.c++; break;
            case 'G': case 'g': counts.g++; break;
            case 'T': case 't': counts.t++; break;
            case 'N': case 'n': counts.n++; break;
            default: counts.other++; break;
        }
    }
    return counts;
}

/**
 * @brief Calculate GC content
 */
double gcContent(const std::string& seq) {
    if (seq.empty()) return 0.0;
    
    auto counts = countBases(seq);
    size_t gc = counts.g + counts.c;
    size_t total = counts.a + counts.c + counts.g + counts.t;
    
    if (total == 0) return 0.0;
    return static_cast<double>(gc) / static_cast<double>(total);
}

/**
 * @brief Reverse complement a sequence
 */
std::string reverseComplement(const std::string& seq) {
    std::string result;
    result.reserve(seq.size());
    
    for (auto it = seq.rbegin(); it != seq.rend(); ++it) {
        char c = *it;
        switch (c) {
            case 'A': result += 'T'; break;
            case 'T': result += 'A'; break;
            case 'C': result += 'G'; break;
            case 'G': result += 'C'; break;
            case 'a': result += 't'; break;
            case 't': result += 'a'; break;
            case 'c': result += 'g'; break;
            case 'g': result += 'c'; break;
            case 'N': case 'n': result += c; break;
            default: result += 'N'; break;
        }
    }
    return result;
}

/**
 * @brief Trim low quality bases from ends
 */
std::pair<size_t, size_t> findTrimPositions(const std::string& qual, int threshold) {
    if (qual.empty()) return {0, 0};
    
    size_t start = 0;
    size_t end = qual.size();
    
    // Trim from start
    while (start < qual.size() && qualityToPhred(qual[start]) < threshold) {
        start++;
    }
    
    // Trim from end
    while (end > start && qualityToPhred(qual[end - 1]) < threshold) {
        end--;
    }
    
    return {start, end};
}

/**
 * @brief Find positions of N bases
 */
std::vector<size_t> findNPositions(const std::string& seq) {
    std::vector<size_t> positions;
    for (size_t i = 0; i < seq.size(); ++i) {
        if (seq[i] == 'N' || seq[i] == 'n') {
            positions.push_back(i);
        }
    }
    return positions;
}

}  // namespace

/**
 * @brief LibFuzzer entry point
 */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Limit input size
    if (size > 100 * 1024) {
        return 0;
    }

    std::string input(reinterpret_cast<const char*>(data), size);

    // Test quality score operations
    {
        double avg = averageQuality(input);
        (void)avg;
        
        // Convert each character
        for (char c : input) {
            int phred = qualityToPhred(c);
            char back = phredToQuality(phred);
            (void)back;
        }
    }

    // Test base counting
    {
        auto counts = countBases(input);
        volatile size_t total = counts.a + counts.c + counts.g + counts.t + counts.n + counts.other;
        (void)total;
    }

    // Test GC content
    {
        double gc = gcContent(input);
        (void)gc;
    }

    // Test reverse complement
    {
        std::string rc = reverseComplement(input);
        
        // Double reverse complement should give original (for valid bases)
        std::string rc2 = reverseComplement(rc);
        (void)rc2;
    }

    // Test trimming
    {
        for (int threshold = 0; threshold <= 40; threshold += 10) {
            auto [start, end] = findTrimPositions(input, threshold);
            
            if (end > start && end <= input.size()) {
                std::string trimmed = input.substr(start, end - start);
                (void)trimmed;
            }
        }
    }

    // Test N position finding
    {
        auto positions = findNPositions(input);
        for (size_t pos : positions) {
            if (pos < input.size()) {
                volatile char c = input[pos];
                (void)c;
            }
        }
    }

    // Test string splitting (simulating header parsing)
    {
        std::vector<std::string> parts;
        size_t start = 0;
        
        for (size_t i = 0; i <= input.size(); ++i) {
            if (i == input.size() || input[i] == ' ' || input[i] == '\t') {
                if (i > start) {
                    parts.push_back(input.substr(start, i - start));
                }
                start = i + 1;
            }
        }
        
        // Access parts
        for (const auto& part : parts) {
            volatile size_t len = part.length();
            (void)len;
        }
    }

    return 0;
}
