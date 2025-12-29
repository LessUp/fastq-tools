#include "test_helpers.h"

#include <algorithm>
#include <random>
#include <sstream>

namespace fq::test {

std::vector<std::filesystem::path> TestHelpers::tempPaths_;

std::filesystem::path TestHelpers::createTempFile(const std::string& content,
                                                  const std::string& suffix) {
    auto tempDir = std::filesystem::temp_directory_path();
    auto tempFile =
        tempDir / ("fastqtools_test_" + std::to_string(std::random_device{}()) + suffix);

    std::ofstream file(tempFile);
    file << content;
    file.close();

    tempPaths_.push_back(tempFile);
    return tempFile;
}

std::filesystem::path TestHelpers::createTempDir() {
    auto tempDir = std::filesystem::temp_directory_path();
    auto testDir = tempDir / ("fastqtools_test_dir_" + std::to_string(std::random_device{}()));

    std::filesystem::create_directories(testDir);
    tempPaths_.push_back(testDir);
    return testDir;
}

std::string TestHelpers::generateFastQRecords(size_t count, size_t readLength) {
    std::ostringstream oss;

    for (size_t i = 0; i < count; ++i) {
        oss << "@read_" << i << "\n";
        oss << generateRandomDNA(readLength) << "\n";
        oss << "+\n";
        oss << generateRandomQuality(readLength) << "\n";
    }

    return oss.str();
}

std::string TestHelpers::generateRandomDNA(size_t length) {
    static const char bases[] = "ATGC";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 3);

    std::string sequence;
    sequence.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        sequence += bases[dis(gen)];
    }

    return sequence;
}

std::string TestHelpers::generateRandomQuality(size_t length, int minQuality, int maxQuality) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(minQuality, maxQuality);

    std::string quality;
    quality.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        quality += static_cast<char>(dis(gen) + 33);  // Phred+33 encoding
    }

    return quality;
}

bool TestHelpers::compareFiles(const std::filesystem::path& file1,
                               const std::filesystem::path& file2) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);

    if (!f1.is_open() || !f2.is_open()) {
        return false;
    }

    return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(f2.rdbuf()));
}

void TestHelpers::cleanup() {
    for (const auto& path : tempPaths_) {
        std::error_code ec;
        if (std::filesystem::is_directory(path)) {
            std::filesystem::remove_all(path, ec);
        } else {
            std::filesystem::remove(path, ec);
        }
    }
    tempPaths_.clear();
}

void FastQToolsTest::SetUp() {
    tempDir_ = TestHelpers::createTempDir();
    testDataDir_ = std::filesystem::current_path() / "tests" / "fixtures";
}

void FastQToolsTest::TearDown() {
    TestHelpers::cleanup();
}

}  // namespace fq::test
