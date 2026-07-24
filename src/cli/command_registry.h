/**
 * @file command_registry.h
 * @brief 命令注册表
 * @details 统一管理 CLI 子命令的注册和分发，消除分散的命令注册代码。
 *
 * @author LessUp
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "commands/command_interface.h"

namespace fq::cli {

/**
 * @brief 命令注册表
 * @details 集中管理所有 CLI 子命令的注册和分发。
 *          消除 main.cpp 中分散的命令注册代码，提高局部性。
 */
class CommandRegistry {
public:
    CommandRegistry() = default;

    /**
     * @brief 注册命令
     * @param name 命令名称
     * @param command 命令实例
     */
    auto registerCommand(std::string name, CommandPtr command) -> void {
        commands_[std::move(name)] = std::move(command);
    }

    /**
     * @brief 查找命令
     * @param name 命令名称
     * @return 命令指针，如果未找到返回 nullptr
     */
    [[nodiscard]] auto findCommand(const std::string& name) const -> CommandInterface* {
        auto it = commands_.find(name);
        return it != commands_.end() ? it->second.get() : nullptr;
    }

    /**
     * @brief 检查命令是否存在
     * @param name 命令名称
     * @return 是否存在
     */
    [[nodiscard]] auto hasCommand(const std::string& name) const -> bool {
        return commands_.find(name) != commands_.end();
    }

    /**
     * @brief 获取所有命令名称
     * @return 命令名称列表
     */
    [[nodiscard]] auto getCommandNames() const -> std::vector<std::string> {
        std::vector<std::string> names;
        names.reserve(commands_.size());
        for (const auto& [name, _] : commands_) {
            names.push_back(name);
        }
        return names;
    }

    /**
     * @brief 执行命令
     * @param name 命令名称
     * @param argc 参数数量
     * @param argv 参数数组
     * @return 命令退出码
     */
    auto executeCommand(const std::string& name, int argc, char* argv[]) const -> int {
        auto* command = findCommand(name);
        if (!command) {
            return -1;  // 命令未找到
        }
        return command->execute(argc, argv);
    }

    /**
     * @brief 打印全局帮助信息
     * @param out 输出流
     */
    auto printHelp(std::ostream& out) const -> void {
        out << "Usage: FastQTools [global-options] <command> [options]\n\n"
            << "Global options:\n"
            << "  -v, --verbose        Enable verbose/debug output\n"
            << "  -q, --quiet          Suppress non-error output\n"
            << "  --log-level=LEVEL    Set log level (trace,debug,info,warn,error)\n\n"
            << "Available commands:\n";

        for (const auto& [name, command] : commands_) {
            out << "  " << name << "\t\t" << command->getDescription() << "\n";
        }

        out << "\nRun 'FastQTools <command> --help' for more information on a specific command.\n";
    }

private:
    std::map<std::string, CommandPtr> commands_;
};

/**
 * @brief 创建默认命令注册表
 * @details 注册所有内置命令（stat, filter）
 * @return 命令注册表实例
 */
auto createDefaultCommandRegistry() -> std::unique_ptr<CommandRegistry>;

}  // namespace fq::cli
