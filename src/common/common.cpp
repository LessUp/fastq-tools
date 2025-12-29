#include "fqtools/common/common.h"

#include <atomic>
#include <ctime>
#include <iostream>
#include <sstream>
#include <utility>

#include <fmt/format.h>

namespace fq::common {

Timer::Timer(std::string_view name)
    : name_(name), start_(std::chrono::high_resolution_clock::now()) {}

void Timer::report() const {
    auto elapsed = std::chrono::high_resolution_clock::now() - start_;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    std::cout << fmt::format("[{}] Elapsed: {}ms\n", name_, milliseconds);
}

auto Timer::elapsed() const -> std::chrono::nanoseconds {
    return std::chrono::high_resolution_clock::now() - start_;
}

auto split(std::string_view input, char delimiter) -> std::vector<std::string> {
    std::vector<std::string> result;
    std::istringstream stream{std::string(input)};
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        if (!token.empty()) {
            result.push_back(token);
        }
    }
    return result;
}

auto trim(std::string_view input) -> std::string {
    auto start = input.find_first_not_of(" \t\n\r");
    if (start == std::string_view::npos)
        return "";
    auto end = input.find_last_not_of(" \t\n\r");
    return std::string(input.substr(start, end - start + 1));
}

auto join(const std::vector<std::string>& parts, std::string_view delimiter) -> std::string {
    if (parts.empty())
        return "";
    std::ostringstream result;
    result << parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result << delimiter << parts[i];
    }
    return result.str();
}

auto Logger::instance() -> Logger& {
    static Logger logger;
    return logger;
}

void Logger::log(Level level, std::string_view message) {
    if (level >= level_) {
        std::cout << fmt::format(
            "[{}] {}: {}\n", getLevelString(level), getCurrentTime(), message);
    }
}

void Logger::setLevel(Level level) {
    level_ = level;
}

auto Logger::getLevelString(Level level) -> std::string_view {
    switch (level) {
        case Level::Debug:
            return "DEBUG";
        case Level::Info:
            return "INFO";
        case Level::Warn:
            return "WARN";
        case Level::Error:
            return "ERROR";
        case Level::Critical:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

auto Logger::getCurrentTime() -> std::string {
    auto now = std::chrono::system_clock::now();
    auto timeTNow = std::chrono::system_clock::to_time_t(now);
    struct tm tmBuf;
#ifdef _WIN32
    localtime_s(&tmBuf, &timeTNow);
#else
    localtime_r(&timeTNow, &tmBuf);
#endif

    return fmt::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}",
                       tmBuf.tm_year + 1900,
                       tmBuf.tm_mon + 1,
                       tmBuf.tm_mday,
                       tmBuf.tm_hour,
                       tmBuf.tm_min,
                       tmBuf.tm_sec);
}

auto IDGenerator::nextId() -> ID {
    static std::atomic<ID> counter{1};
    return counter.fetch_add(1, std::memory_order_relaxed);
}

auto IDGenerator::reset() -> void {
    static std::atomic<ID> counter{1};
    counter.store(1, std::memory_order_relaxed);
}

void printSoftwareInfo() {
    std::cout << "FASTQTOOLS v3.1.0 - Modern C++20 FastQ Processing Tool\n";
    std::cout << "Copyright (c) 2025 LessUp\n";
    std::cout << "Built with modern C++ modules and high-performance parallel processing\n";
}

void printLogo() {
    std::cout << R"(
    ███████╗ █████╗ ███████╗████████╗ ██████╗ ████████╗ ██████╗  ██████╗ ██║     ███████╗
    ██╔════╝██╔══██╗██╔════╝╚══██╔══╝██╔═══██╗╚══██╔══╝██╔═══██╗██╔═══██╗██║     ██╔════╝
    █████╗  ███████║███████╗   ██║   ██║   ██║   ██║   ██║   ██║██║   ██║██║     ███████╗
    ██╔══╝  ██╔══██║╚════██║   ██║   ██║▄▄ ██║   ██║   ██║   ██║██║   ██║██║     ╚════██║
    ██║     ██║  ██║███████║   ██║   ╚██████╔╝   ██║   ╚██████╔╝╚██████╔╝███████╗███████║
    ╚═╝     ╚═╝  ╚═╝╚══════╝   ╚═╝    ╚══▀▀═╝    ╚═╝    ╚═════╝  ╚═════╝ ╚══════╝╚══════╝
        )" << std::endl;
}
}  // namespace fq::common
