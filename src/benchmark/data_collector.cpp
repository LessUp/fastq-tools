#include <fqtools/benchmark/data_collector.h>

#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <thread>

#ifdef __linux__
#include <sys/sysinfo.h>
#include <unistd.h>
#endif

namespace fq::benchmark {

namespace {

/**
 * @brief 执行命令并获取输出
 */
std::string execCommand(const char* cmd) {
    std::array<char, 256> buffer{};
    std::string result;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        return "unknown";
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    // 移除尾部换行符
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
        result.pop_back();
    }

    return result.empty() ? "unknown" : result;
}

}  // namespace

SystemMetadata DataCollector::collectSystemMetadata() {
    SystemMetadata metadata;
    metadata.timestamp = getTimestamp();
    metadata.gitCommit = getGitCommit();
    metadata.gitBranch = getGitBranch();
    metadata.cpuModel = getCpuModel();
    metadata.coreCount = getCoreCount();
    metadata.memoryBytes = getMemoryBytes();
    metadata.osVersion = getOsVersion();
    metadata.compilerVersion = getCompilerVersion();
    return metadata;
}

std::string DataCollector::getGitCommit() {
    return execCommand("git rev-parse --short HEAD 2>/dev/null");
}

std::string DataCollector::getGitBranch() {
    return execCommand("git rev-parse --abbrev-ref HEAD 2>/dev/null");
}

std::string DataCollector::getCpuModel() {
#ifdef __linux__
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line)) {
        if (line.find("model name") != std::string::npos) {
            auto pos = line.find(':');
            if (pos != std::string::npos) {
                std::string model = line.substr(pos + 1);
                // 去除前导空格
                while (!model.empty() && model.front() == ' ') {
                    model.erase(0, 1);
                }
                return model;
            }
        }
    }
#endif
    return execCommand("uname -p 2>/dev/null");
}

int DataCollector::getCoreCount() {
    return static_cast<int>(std::thread::hardware_concurrency());
}

std::size_t DataCollector::getMemoryBytes() {
#ifdef __linux__
    struct sysinfo info {};
    if (sysinfo(&info) == 0) {
        return static_cast<std::size_t>(info.totalram) * info.mem_unit;
    }
#endif
    return 0;
}

std::string DataCollector::getOsVersion() {
#ifdef __linux__
    // 尝试读取 /etc/os-release
    std::ifstream osRelease("/etc/os-release");
    std::string line;
    while (std::getline(osRelease, line)) {
        if (line.find("PRETTY_NAME=") == 0) {
            std::string name = line.substr(12);
            // 移除引号
            if (!name.empty() && name.front() == '"') {
                name.erase(0, 1);
            }
            if (!name.empty() && name.back() == '"') {
                name.pop_back();
            }
            return name;
        }
    }
#endif
    return execCommand("uname -sr 2>/dev/null");
}

std::string DataCollector::getCompilerVersion() {
#if defined(__clang__)
    return "clang " + std::to_string(__clang_major__) + "." +
           std::to_string(__clang_minor__) + "." +
           std::to_string(__clang_patchlevel__);
#elif defined(__GNUC__)
    return "gcc " + std::to_string(__GNUC__) + "." +
           std::to_string(__GNUC_MINOR__) + "." +
           std::to_string(__GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    return "msvc " + std::to_string(_MSC_VER);
#else
    return "unknown";
#endif
}

std::string DataCollector::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    gmtime_r(&time, &tm);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

std::string DataCollector::toJson(const BenchmarkReport& report) {
    nlohmann::json j = report;
    return j.dump(2);
}

BenchmarkReport DataCollector::fromJson(const std::string& json) {
    auto j = nlohmann::json::parse(json);
    return j.get<BenchmarkReport>();
}

std::string DataCollector::generateResultFilename(const SystemMetadata& metadata) {
    // 从 ISO 时间戳提取日期时间部分
    std::string ts = metadata.timestamp;
    // 替换 T 和 : 为 _ 和 -
    for (char& c : ts) {
        if (c == 'T') {
            c = '_';
        }
        if (c == ':') {
            c = '-';
        }
    }
    // 移除 Z 后缀
    if (!ts.empty() && ts.back() == 'Z') {
        ts.pop_back();
    }

    return ts + "_" + metadata.gitCommit + ".json";
}

}  // namespace fq::benchmark
