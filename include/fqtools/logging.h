#pragma once
/**
 * @file logging.h
 * @brief 统一日志接口（基于 fmt 的轻量实现）
 * @note 内部头：不安装、不被 fq.h 聚合，仅供项目内部 .cpp 使用。
 */

#include <atomic>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>

namespace fq::logging {

/// @brief 日志级别
enum class Level : int {
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Off = 5,
};

/// @brief 全局日志级别
inline std::atomic<Level> currentLevel{Level::Info};

/// @brief 解析日志级别字符串
/// @throws std::invalid_argument 未知级别名（拼错的 --log-level 应在参数阶段报错，
///         静默回退会让用户以为生效了实际没有）
inline auto parseLevel(std::string_view name) -> Level {
    if (name == "debug") {
        return Level::Debug;
    }
    if (name == "info") {
        return Level::Info;
    }
    if (name == "warn" || name == "warning") {
        return Level::Warn;
    }
    if (name == "error") {
        return Level::Error;
    }
    if (name == "off") {
        return Level::Off;
    }
    throw std::invalid_argument("unknown log level: " + std::string(name));
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
inline void logImpl(Level level,
                    std::string_view tag,
                    fmt::format_string<Args...> fmtStr,
                    Args&&... args) {
    if (currentLevel.load(std::memory_order_relaxed) > level) {
        return;
    }
    fmt::print(stderr, "[{}] {}\n", tag, fmt::format(fmtStr, std::forward<Args>(args)...));
}

}  // namespace detail

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

}  // namespace fq::logging
