#include "fixture_loader.h"

#include <algorithm>
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
    auto fixture_path = current_path / "tests" / "fixtures" / filename;

    if (!std::filesystem::exists(fixture_path)) {
        // 尝试从构建目录查找
        fixture_path = current_path / "fixtures" / filename;
    }

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

}  // namespace fq::test
