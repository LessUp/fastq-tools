/**
 * @file config.h
 * @brief 提供了一个灵活的配置管理系统。
 * @author LessUp
 * @version 1.0
 * @date 2025-07-31
 * @copyright Copyright (c) 2025 LessUp
 */

#pragma once

#include <cstdint>
#include <filesystem>
#include <format>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "fqtools/error/error.h"

namespace fq::config {

/// @brief 配置项的值类型，可以是 bool, int64, double, 或 string。
using ConfigValue = std::variant<bool, std::int64_t, double, std::string>;

/**
 * @brief 负责加载、管理和验证项目配置。
 * @details 可以从文件、命令行参数和环境变量中加载配置。
 */
class Configuration {
public:
    Configuration() = default;
    /**
     * @brief 从配置文件加载配置
     * @details 从指定的配置文件中读取并解析配置参数
     *
     * @param configFile 配置文件路径
     * @pre configFile 必须指向有效的配置文件
     * @post 配置参数被加载到实例中
     * @throw ConfigurationError 如果文件不存在或格式错误
     */
    void loadFromFile(const std::filesystem::path& configFile);

    /**
     * @brief 从命令行参数加载配置
     * @details 解析命令行参数并加载相应的配置项
     *
     * @param argc 参数个数
     * @param argv 参数数组
     * @pre argv 必须包含有效的命令行参数
     * @post 配置参数被加载到实例中
     * @throw ConfigurationError 如果参数格式错误
     */
    void loadFromArgs(int argc, char* argv[]);

    /**
     * @brief 从环境变量加载配置
     * @details 从系统环境变量中读取并加载配置参数
     *
     * @post 配置参数被加载到实例中
     * @note 只会加载已定义的环境变量
     */
    void loadFromEnv();

    template <typename T>
    [[nodiscard]] auto get(std::string_view key) const -> T {
        auto it = values_.find(std::string(key));
        if (it == values_.end()) {
            throw fq::error::ConfigurationError(std::format("Configuration key '{}' not found", key));
        }
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            throw fq::error::ConfigurationError(std::format("Configuration key '{}' type mismatch", key));
        }
    }

    template <typename T>
    [[nodiscard]] auto getOr(std::string_view key, T defaultValue) const -> T {
        auto it = values_.find(std::string(key));
        if (it == values_.end()) {
            return defaultValue;
        }
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            return defaultValue;
        }
    }

    template <typename T>
    void set(std::string_view key, T value) {
        validateKey(key);
        values_[std::string(key)] = value;
    }

    /**
     * @brief 检查配置键是否存在
     * @details 检查指定的配置键是否存在于当前配置中
     *
     * @param key 配置键名
     * @return 存在返回 true，不存在返回 false
     */
    [[nodiscard]] auto hasKey(std::string_view key) const -> bool;
    [[nodiscard]] auto empty() const -> bool;
    [[nodiscard]] auto size() const -> size_t;
    [[nodiscard]] auto keys() const -> std::vector<std::string>;
    void clear();
    void validate() const;
    void printConfig(std::ostream& out) const;
    void setFromString(const std::string& key, const std::string& value);

private:
    std::unordered_map<std::string, ConfigValue> values_;
    std::unordered_map<char, std::string> shortToLong_ = {{'i', "input"},
                                                         {'o', "output"},
                                                         {'t', "threads"},
                                                         {'m', "memoryLimitMb"},
                                                         {'v', "verbose"},
                                                         {'h', "help"}};
    void validateKey(std::string_view key) const;
    void parseConfigStream(std::istream& stream);
    auto getLongNameForShort(char shortName) const -> std::string;
};

/**
 * @brief 获取全局配置实例
 * @details 获取全局配置管理器的单例实例
 *
 * @return Configuration& 全局配置实例的引用
 * @note 如果未初始化，会自动创建默认配置的实例
 */
auto globalConfig() -> Configuration&;

template <typename T>
auto getConfig(std::string_view key) -> T {
    return globalConfig().get<T>(key);
}
template <typename T>
auto getConfigOr(std::string_view key, T defaultValue) -> T {
    return globalConfig().getOr<T>(key, defaultValue);
}
template <typename T>
void setConfig(std::string_view key, T value) {
    globalConfig().set(key, value);
}
}  // namespace fq::config
