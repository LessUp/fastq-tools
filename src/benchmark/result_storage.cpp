#include "result_storage.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>

#include "data_collector.h"

namespace fq::benchmark {

ResultStorage::ResultStorage(const std::filesystem::path& baseDir)
    : baseDir_(baseDir), resultsDir_(baseDir / "results"), baselinesDir_(baseDir / "baselines") {
    ensureDirectories();
}

void ResultStorage::ensureDirectories() {
    std::filesystem::create_directories(resultsDir_);
    std::filesystem::create_directories(baselinesDir_);
}

std::filesystem::path ResultStorage::saveResult(const BenchmarkReport& report) {
    std::string filename = DataCollector::generateResultFilename(report.metadata);
    std::filesystem::path filepath = resultsDir_ / filename;

    std::string json = DataCollector::toJson(report);
    std::ofstream ofs(filepath);
    if (!ofs) {
        throw std::runtime_error("Failed to open file for writing: " + filepath.string());
    }
    ofs << json;
    ofs.close();

    return filepath;
}

BenchmarkReport ResultStorage::loadResult(const std::string& filename) {
    std::filesystem::path filepath = resultsDir_ / filename;

    std::ifstream ifs(filepath);
    if (!ifs) {
        throw std::runtime_error("Failed to open result file: " + filepath.string());
    }

    std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return DataCollector::fromJson(json);
}

std::vector<std::string> ResultStorage::listResults() const {
    std::vector<std::string> results;

    if (!std::filesystem::exists(resultsDir_)) {
        return results;
    }

    for (const auto& entry : std::filesystem::directory_iterator(resultsDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            results.push_back(entry.path().filename().string());
        }
    }

    // 按文件名排序（时间戳在前，所以按字母序即可）
    std::sort(results.begin(), results.end());
    return results;
}

BenchmarkReport ResultStorage::getLatestResult() {
    auto results = listResults();
    if (results.empty()) {
        throw std::runtime_error("No benchmark results found");
    }
    return loadResult(results.back());
}

std::filesystem::path ResultStorage::saveBaseline(const BenchmarkReport& report,
                                                  const std::string& name) {
    std::filesystem::path filepath = baselinesDir_ / (name + ".json");

    std::string json = DataCollector::toJson(report);
    std::ofstream ofs(filepath);
    if (!ofs) {
        throw std::runtime_error("Failed to open file for writing: " + filepath.string());
    }
    ofs << json;
    ofs.close();

    return filepath;
}

BenchmarkReport ResultStorage::loadBaseline(const std::string& name) {
    std::filesystem::path filepath = baselinesDir_ / (name + ".json");

    std::ifstream ifs(filepath);
    if (!ifs) {
        throw std::runtime_error("Baseline not found: " + name);
    }

    std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return DataCollector::fromJson(json);
}

std::vector<std::string> ResultStorage::listBaselines() const {
    std::vector<std::string> baselines;

    if (!std::filesystem::exists(baselinesDir_)) {
        return baselines;
    }

    for (const auto& entry : std::filesystem::directory_iterator(baselinesDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            std::string name = entry.path().stem().string();
            baselines.push_back(name);
        }
    }

    std::sort(baselines.begin(), baselines.end());
    return baselines;
}

bool ResultStorage::baselineExists(const std::string& name) const {
    std::filesystem::path filepath = baselinesDir_ / (name + ".json");
    return std::filesystem::exists(filepath);
}

}  // namespace fq::benchmark
