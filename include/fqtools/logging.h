#pragma once
/**
 * @file logging.h
 * @brief 统一日志初始化接口
 */

#include <string>

#include <spdlog/spdlog.h>

namespace fq::logging {

/**
 * @brief 日志配置选项
 */
struct LogOptions {
    std::string level = "info";  ///< 日志级别: trace, debug, info, warn, error, critical
    std::string pattern = "";    ///< 自定义日志格式（空则使用默认）
    bool colored = true;         ///< 是否启用彩色输出
};

/**
 * @brief 初始化日志系统
 * @param options 日志配置选项
 */
inline void init(const LogOptions& options = {}) {
    spdlog::level::level_enum level = spdlog::level::info;

    if (options.level == "trace")
        level = spdlog::level::trace;
    else if (options.level == "debug")
        level = spdlog::level::debug;
    else if (options.level == "info")
        level = spdlog::level::info;
    else if (options.level == "warn" || options.level == "warning")
        level = spdlog::level::warn;
    else if (options.level == "error")
        level = spdlog::level::err;
    else if (options.level == "critical")
        level = spdlog::level::critical;
    else if (options.level == "off")
        level = spdlog::level::off;

    spdlog::set_level(level);

    if (!options.pattern.empty()) {
        spdlog::set_pattern(options.pattern);
    }
}

/**
 * @brief 便捷函数：设置日志级别
 * @param level 日志级别字符串
 */
inline void setLevel(const std::string& level) {
    LogOptions opts;
    opts.level = level;
    init(opts);
}

}  // namespace fq::logging
