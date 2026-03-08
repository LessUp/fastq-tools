#include "fixture_loader.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <random>
#include <sstream>

namespace fq::test {

std::string FixtureLoader::loadTextFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open fixture file: " + path.string());
    }

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

std::vector<std::string> FixtureLoader::loadLines(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open fixture file: " + path.string());
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::filesystem::path FixtureLoader::getFixturePath(const std::string& filename) {
    auto current_path = std::filesystem::current_path();

    // 优先查找 tools/data/（项目标准测试数据目录）
    auto fixture_path = current_path / "tools" / "data" / filename;
    if (std::filesystem::exists(fixture_path)) {
        return fixture_path;
    }

    // 回退：从构建目录向上查找
    fixture_path = current_path / ".." / "tools" / "data" / filename;
    return fixture_path;
}

bool FixtureLoader::fixtureExists(const std::string& filename) {
    return std::filesystem::exists(getFixturePath(filename));
}

std::filesystem::path FixtureLoader::createTempFastq(size_t records, size_t read_length) {
    auto temp_dir = std::filesystem::temp_directory_path();
    auto temp_file = temp_dir / ("fastqtools_fixture_" +
                                 std::to_string(std::random_device{}()) + ".fastq");

    std::ofstream out(temp_file);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot create temp FASTQ file: " + temp_file.string());
    }

    static const char bases[] = "ACGT";
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> base_dist(0, 3);
    std::uniform_int_distribution<> qual_dist(20, 40);

    for (size_t i = 0; i < records; ++i) {
        out << "@read_" << i << '\n';
        for (size_t j = 0; j < read_length; ++j) {
            out << bases[base_dist(gen)];
        }
        out << '\n' << "+\n";
        for (size_t j = 0; j < read_length; ++j) {
            out << static_cast<char>(qual_dist(gen) + 33);
        }
        out << '\n';
    }

    return temp_file;
}

bool FixtureLoader::compareFiles(const std::filesystem::path& file1,
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

// ---------------------------------------------------------------------------
// PerformanceTimer 实现
// ---------------------------------------------------------------------------

PerformanceTimer::PerformanceTimer() : running_(false) {}

void PerformanceTimer::start() {
    start_time_ = std::chrono::high_resolution_clock::now();
    running_ = true;
}

void PerformanceTimer::stop() {
    end_time_ = std::chrono::high_resolution_clock::now();
    running_ = false;
}

void PerformanceTimer::reset() {
    running_ = false;
}

double PerformanceTimer::elapsedSeconds() const {
    auto end = running_ ? std::chrono::high_resolution_clock::now() : end_time_;
    return std::chrono::duration<double>(end - start_time_).count();
}

double PerformanceTimer::elapsedMilliseconds() const {
    return elapsedSeconds() * 1000.0;
}

// ---------------------------------------------------------------------------
// TempDirectory 实现
// ---------------------------------------------------------------------------

TempDirectory::TempDirectory()
    : TempDirectory("fastqtools_test_") {}

TempDirectory::TempDirectory(const std::string& prefix) {
    auto base = std::filesystem::temp_directory_path();
    path_ = base / (prefix + std::to_string(std::random_device{}()));
    std::filesystem::create_directories(path_);
}

TempDirectory::~TempDirectory() {
    if (!path_.empty() && std::filesystem::exists(path_)) {
        std::error_code ec;
        std::filesystem::remove_all(path_, ec);
    }
}

TempDirectory::TempDirectory(TempDirectory&& other) noexcept
    : path_(std::move(other.path_)) {
    other.path_.clear();
}

TempDirectory& TempDirectory::operator=(TempDirectory&& other) noexcept {
    if (this != &other) {
        if (!path_.empty() && std::filesystem::exists(path_)) {
            std::error_code ec;
            std::filesystem::remove_all(path_, ec);
        }
        path_ = std::move(other.path_);
        other.path_.clear();
    }
    return *this;
}

}  // namespace fq::test
