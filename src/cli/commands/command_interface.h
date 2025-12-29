/**
 * @file command_interface.h
 * @brief 定义了命令行命令的抽象接口。
 * @author LessUp
 * @version 1.0
 * @date 2025-07-31
 * @copyright Copyright (c) 2025 LessUp
 */

#pragma once
#include <memory>
#include <string>

namespace cxxopts {
class Options;
class ParseResult;
}  // namespace cxxopts

namespace fq::cli {

/**
 * @brief 命令行命令的抽象基类。
 * @details 所有具体的命令（如 `stat`, `filter`）都必须从此接口继承，
 *          并实现其纯虚函数。
 */
class CommandInterface {
public:
    /**
     * @brief 虚析构函数。
     */
    virtual ~CommandInterface() = default;

    /**
     * @brief 执行命令的核心逻辑。
     * @param argc 参数数量。
     * @param argv 参数值数组。
     * @return int 命令的退出码，0 表示成功。
     */
    virtual auto execute(int argc, char* argv[]) -> int = 0;

    /**
     * @brief 获取命令的名称。
     * @return std::string 命令的名称 (例如 "stat")。
     */
    [[nodiscard]] virtual auto getName() const -> std::string = 0;

    /**
     * @brief 获取命令的简要描述。
     * @return std::string 命令功能的简要描述。
     */
    [[nodiscard]] virtual auto getDescription() const -> std::string = 0;
};

/// @brief 指向 CommandInterface 实例的唯一指针类型别名。
using CommandPtr = std::unique_ptr<CommandInterface>;

}  // namespace fq::cli
