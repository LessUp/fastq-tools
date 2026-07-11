#pragma once
/**
 * @file logging.h
 * @brief 统一日志接口（基于 fmt 的轻量实现）
 */

#include <atomic>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>

namespace fq::logging {

/// @brief 日志级别
enum class Level : int {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Critical = 5,
    Off = 6
};

/// @brief 全局日志级别
inline std::atomic<Level> currentLevel{Level::Info};

/// @brief 日志配置选项
struct LogOptions {
    std::string level = "info";  ///< 日志级别: trace, debug, info, warn, error, critical, off
    std::string pattern = "";    ///< 保留兼容，当前实现忽略
    bool colored = true;         ///< 保留兼容，当前实现忽略
};

/// @brief 解析日志级别字符串
inline auto parseLevel(std::string_view name) -> Level {
    if (name == "trace") return Level::Trace;
    if (name == "debug") return Level::Debug;
    if (name == "info") return Level::Info;
    if (name == "warn" || name == "warning") return Level::Warn;
    if (name == "error") return Level::Error;
    if (name == "critical") return Level::Critical;
    if (name == "off") return Level::Off;
    return Level::Info;
}

/// @brief 初始化日志系统
/// @param options 日志配置选项
inline void init(const LogOptions& options = {}) {
    currentLevel.store(parseLevel(options.level), std::memory_order_relaxed);
}

/// @brief 便捷函数：设置日志级别
/// @param level 日志级别字符串
inline void setLevel(const std::string& level) {
    currentLevel.store(parseLevel(level), std::memory_order_relaxed);
}

/// @brief 获取当前日志级别
inline auto getLevel() -> Level {
    return currentLevel.load(std::memory_order_relaxed);
}

namespace detail {

template <typename... Args>
inline void logImpl(Level level, std::string_view tag, fmt::format_string<Args...> fmtStr,
                    Args&&... args) {
    if (currentLevel.load(std::memory_order_relaxed) > level) return;
    fmt::print(stderr, "[{}] {}\n", tag, fmt::format(fmtStr, std::forward<Args>(args)...));
}

}  // namespace detail

template <typename... Args>
inline void trace(fmt::format_string<Args...> fmt, Args&&... args) {
    detail::logImpl(Level::Trace, "trace", fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(fmt::format_string<Args...> fmt, Args&&... args) {
    detail::logImpl(Level::Debug, "debug", fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(fmt::format_string<Args...> fmt, Args&&... args) {
    detail::logImpl(Level::Info, "info", fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(fmt::format_string<Args...> fmt, Args&&... args) {
    detail::logImpl(Level::Warn, "warn", fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(fmt::format_string<Args...> fmt, Args&&... args) {
    detail::logImpl(Level::Error, "error", fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void critical(fmt::format_string<Args...> fmt, Args&&... args) {
    detail::logImpl(Level::Critical, "critical", fmt, std::forward<Args>(args)...);
}

}  // namespace fq::logging
