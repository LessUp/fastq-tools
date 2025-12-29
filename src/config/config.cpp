/**
 * @file config.cpp
 * @brief 配置模块实现文件
 * @details 实现了 Configuration 类的主要功能，包括文件和命令行参数解析。
 * @author FastQTools Team
 * @date 2025-08-01
 * @version 1.0
 * @copyright Copyright (c) 2025 FastQTools
 */

#include "fqtools/config/config.h"

#include "fqtools/common/common.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <format>
#include <fstream>
#include <istream>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>

#include "fqtools/error/error.h"

extern char** environ;

namespace fq::config {

/**
 * @brief 从配置文件加载参数
 * @details 检查文件存在性并解析内容，异常时抛出配置错误
 * @param configFile 配置文件路径
 */
void Configuration::loadFromFile(const std::filesystem::path& configFile) {
    if (!std::filesystem::exists(configFile)) {
        FQ_THROW_CONFIG_ERROR(
            std::format("Configuration file '{}' does not exist", configFile.string()));
    }
    std::ifstream file(configFile);
    if (!file) {
        FQ_THROW_CONFIG_ERROR(
            std::format("Cannot open configuration file '{}'", configFile.string()));
    }
    parseConfigStream(file);
}

/**
 * @brief 从命令行参数加载配置
 * @details 支持多种参数格式，包括 --key=value、--key、-k value
 * @param argc 参数个数
 * @param argv 参数数组
 */
void Configuration::loadFromArgs(int argc, char* argv[]) {
    // 解析命令行参数。
    // 支持三种格式:
    // 1. --key=value
    // 2. --key (布尔值 true)
    // 3. -k value (短名称)
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.starts_with("--")) {
            auto eqPos = arg.find('=');
            if (eqPos != std::string::npos) {
                auto key = arg.substr(2, eqPos - 2);
                auto value = arg.substr(eqPos + 1);
                setFromString(key, value);
            } else {
                auto key = arg.substr(2);
                if (i + 1 < argc && !std::string(argv[i + 1]).starts_with("-")) {
                    setFromString(key, argv[++i]);
                } else {
                    set(key, true);
                }
            }
        } else if (arg.starts_with("-") && arg.length() == 2) {
            char shortName = arg[1];
            auto key = getLongNameForShort(shortName);
            if (!key.empty()) {
                if (i + 1 < argc && !std::string(argv[i + 1]).starts_with("-")) {
                    setFromString(key, argv[++i]);
                } else {
                    set(key, true);
                }
            }
        }
    }
}

void Configuration::loadFromEnv() {
    // 从环境变量加载配置。
    // 只加载以 "FASTQTOOLS_" 为前缀的变量，并将其余部分转换为小写作为键。
    const char* envPrefix = "FASTQTOOLS_";
    for (char** env = environ; *env != nullptr; ++env) {
        std::string envVar(*env);
        auto eqPos = envVar.find('=');
        if (eqPos != std::string::npos) {
            auto key = envVar.substr(0, eqPos);
            auto value = envVar.substr(eqPos + 1);
            if (key.starts_with(envPrefix)) {
                auto configKey = key.substr(std::strlen(envPrefix));
                std::transform(configKey.begin(), configKey.end(), configKey.begin(), ::tolower);
                setFromString(configKey, value);
            }
        }
    }
}

void Configuration::validate() const {
    static const std::vector<std::string> requiredKeys = {"input", "output"};
    for (const auto& key : requiredKeys) {
        if (!hasKey(key)) {
            FQ_THROW_CONFIG_ERROR(std::format("Required configuration key '{}' is missing", key));
        }
    }
    if (hasKey("threads")) {
        auto threads = getOr<std::int64_t>("threads", 1);
        if (threads <= 0 || threads > 256) {
            FQ_THROW_CONFIG_ERROR("threads must be between 1 and 256");
        }
    }
    if (hasKey("memoryLimitMb")) {
        auto memory = getOr<std::int64_t>("memoryLimitMb", 1024);
        if (memory <= 0 || memory > 1024 * 1024) {
            FQ_THROW_CONFIG_ERROR("memoryLimitMb must be between 1 and 1048576 MB");
        }
    }
}

void Configuration::printConfig(std::ostream& out) const {
    out << "Current Configuration:\n=====================\n";
    for (const auto& [key, value] : values_) {
        out << std::format("{:20}: ", key);
        std::visit(
            [&out](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, bool>) {
                    out << (v ? "true" : "false");
                } else {
                    out << v;
                }
            },
            value);
        out << '\n';
    }
}

auto Configuration::hasKey(std::string_view key) const -> bool {
    return values_.find(std::string(key)) != values_.end();
}

auto Configuration::empty() const -> bool {
    return values_.empty();
}

auto Configuration::size() const -> size_t {
    return values_.size();
}

auto Configuration::keys() const -> std::vector<std::string> {
    std::vector<std::string> keys;
    keys.reserve(values_.size());
    for (const auto& [key, _] : values_) {
        keys.push_back(key);
    }
    return keys;
}

void Configuration::clear() {
    values_.clear();
}

void Configuration::validateKey(std::string_view key) const {
    if (key.empty()) {
        FQ_THROW_CONFIG_ERROR("Configuration key cannot be empty");
    }
    for (char character : key) {
        if (!std::isalnum(character) && character != '_' && character != '-') {
            FQ_THROW_CONFIG_ERROR(
                std::format("Invalid character '{}' in configuration key '{}'", character, key));
        }
    }
}

void Configuration::parseConfigStream(std::istream& stream) {
    std::string line;
    int lineNumber = 0;
    while (std::getline(stream, line)) {
        ++lineNumber;
        // 忽略 '#' 之后的注释
        auto commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        auto trimmed = fq::common::trim(line);
        if (trimmed.empty()) {
            continue;
        }

        auto eqPos = trimmed.find('=');
        if (eqPos == std::string::npos) {
            FQ_THROW_CONFIG_ERROR(std::format("Invalid configuration line {}: '{}'", lineNumber, trimmed));
        }

        auto key = fq::common::trim(trimmed.substr(0, eqPos));
        auto value = fq::common::trim(trimmed.substr(eqPos + 1));
        setFromString(key, value);
    }
}

void Configuration::setFromString(const std::string& key, const std::string& value) {
    validateKey(key);
    
    std::string processedValue = value;
    // 去除两端引号 (单引号 or 双引号)
    if (processedValue.size() >= 2 && 
        ((processedValue.front() == '"' && processedValue.back() == '"') ||
         (processedValue.front() == '\'' && processedValue.back() == '\''))) {
        processedValue = processedValue.substr(1, processedValue.size() - 2);
    }

    // 尝试解析为布尔值
    std::string lowerValue = processedValue;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    if (lowerValue == "true" || lowerValue == "yes" || lowerValue == "on") {
        set(key, true);
    } else if (lowerValue == "false" || lowerValue == "no" || lowerValue == "off") {
        set(key, false);
    } else {
        // 尝试解析为数字
        try {
            size_t pos;
            std::int64_t i = std::stoll(processedValue, &pos);
            if (pos == processedValue.length()) {
                set(key, i);
                return;
            }
        } catch (...) {}

        try {
            size_t pos;
            double d = std::stod(processedValue, &pos);
            if (pos == processedValue.length()) {
                set(key, d);
                return;
            }
        } catch (...) {}

        // 最后作为字符串
        set(key, processedValue);
    }
}

auto Configuration::getLongNameForShort(char shortName) const -> std::string {
    auto it = shortToLong_.find(shortName);
    return it != shortToLong_.end() ? it->second : "";
}

auto globalConfig() -> Configuration& {
    static Configuration instance;
    return instance;
}

}  // namespace fq::config
