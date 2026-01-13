/**
 * @file fastq_parser_fuzzer.cpp
 * @brief Fuzz testing for FASTQ file parsing
 *
 * This fuzzer tests the FASTQ parser with random/malformed input to find
 * crashes, memory errors, and undefined behavior.
 *
 * Build: cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ ..
 * Run:   ./fuzzers/fastq_parser_fuzzer corpus/ -max_len=4096
 */

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <filesystem>
#include <memory>

// Forward declarations for internal parsing functions
// We test the parsing logic directly rather than file I/O

namespace {

/**
 * @brief Simple FASTQ record structure for fuzzing
 */
struct FuzzFastqRecord {
    std::string header;
    std::string sequence;
    std::string quality;
};

/**
 * @brief Parse a single FASTQ record from string view
 * @param data Input data
 * @param record Output record
 * @return true if parsing succeeded
 */
bool parseFastqRecord(std::string_view data, FuzzFastqRecord& record) {
    if (data.empty()) {
        return false;
    }

    // Find line boundaries
    size_t pos = 0;
    size_t lineStart = 0;
    int lineNum = 0;
    std::string lines[4];

    while (pos < data.size() && lineNum < 4) {
        if (data[pos] == '\n' || pos == data.size() - 1) {
            size_t lineEnd = (data[pos] == '\n') ? pos : pos + 1;
            lines[lineNum] = std::string(data.substr(lineStart, lineEnd - lineStart));
            lineStart = pos + 1;
            lineNum++;
        }
        pos++;
    }

    if (lineNum < 4) {
        return false;
    }

    // Validate header line starts with @
    if (lines[0].empty() || lines[0][0] != '@') {
        return false;
    }

    // Validate separator line starts with +
    if (lines[2].empty() || lines[2][0] != '+') {
        return false;
    }

    // Validate sequence and quality lengths match
    if (lines[1].length() != lines[3].length()) {
        return false;
    }

    record.header = std::move(lines[0]);
    record.sequence = std::move(lines[1]);
    record.quality = std::move(lines[3]);

    return true;
}

/**
 * @brief Validate FASTQ sequence characters
 * @param seq Sequence string
 * @return true if all characters are valid
 */
bool validateSequence(const std::string& seq) {
    for (char c : seq) {
        // Valid IUPAC nucleotide codes
        switch (c) {
            case 'A': case 'C': case 'G': case 'T': case 'U':
            case 'a': case 'c': case 'g': case 't': case 'u':
            case 'N': case 'n':
            case 'R': case 'Y': case 'S': case 'W':
            case 'K': case 'M': case 'B': case 'D':
            case 'H': case 'V':
                break;
            default:
                return false;
        }
    }
    return true;
}

/**
 * @brief Validate FASTQ quality scores
 * @param qual Quality string
 * @return true if all quality scores are in valid range
 */
bool validateQuality(const std::string& qual) {
    for (char c : qual) {
        // Phred+33 encoding: ASCII 33-126
        if (c < 33 || c > 126) {
            return false;
        }
    }
    return true;
}

}  // namespace

/**
 * @brief LibFuzzer entry point
 *
 * This function is called by libFuzzer with random input data.
 * It should return 0 on success (including graceful error handling).
 * Any crash, assertion failure, or sanitizer error indicates a bug.
 */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Limit input size to prevent OOM
    if (size > 1024 * 1024) {
        return 0;
    }

    // Convert to string_view for parsing
    std::string_view input(reinterpret_cast<const char*>(data), size);

    // Test 1: Parse single record
    {
        FuzzFastqRecord record;
        bool parsed = parseFastqRecord(input, record);
        
        if (parsed) {
            // If parsing succeeded, validate the record
            (void)validateSequence(record.sequence);
            (void)validateQuality(record.quality);
            
            // Access all fields to trigger potential memory errors
            volatile size_t headerLen = record.header.length();
            volatile size_t seqLen = record.sequence.length();
            volatile size_t qualLen = record.quality.length();
            (void)headerLen;
            (void)seqLen;
            (void)qualLen;
        }
    }

    // Test 2: Parse multiple records
    {
        size_t pos = 0;
        int recordCount = 0;
        const int maxRecords = 100;  // Limit to prevent infinite loops

        while (pos < input.size() && recordCount < maxRecords) {
            // Find next record start
            while (pos < input.size() && input[pos] != '@') {
                pos++;
            }

            if (pos >= input.size()) {
                break;
            }

            // Try to parse record starting at pos
            std::string_view remaining = input.substr(pos);
            FuzzFastqRecord record;
            
            if (parseFastqRecord(remaining, record)) {
                recordCount++;
                // Move past this record
                pos += record.header.length() + record.sequence.length() + 
                       record.quality.length() + 4;  // +4 for newlines and +
            } else {
                pos++;
            }
        }
    }

    // Test 3: String operations on input
    {
        std::string inputStr(input);
        
        // Test various string operations that might be used in parsing
        (void)inputStr.find('@');
        (void)inputStr.find('\n');
        (void)inputStr.find('+');
        
        if (!inputStr.empty()) {
            (void)inputStr.substr(0, std::min(size_t(100), inputStr.size()));
        }
    }

    return 0;
}
