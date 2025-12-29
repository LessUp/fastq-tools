/**
 * @file main.cpp
 * @brief FastQTools 命令行主程序入口
 * @details 负责解析命令行参数，分发子命令，并输出帮助信息。
 * @author FastQTools Team
 * @date 2025-08-01
 * @version 1.0
 * @copyright Copyright (c) 2025 FastQTools
 */

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <fqtools/fq.h>
#include <spdlog/spdlog.h>
#include "commands/command_interface.h"
#include "commands/filter_command.h"
#include "commands/stat_command.h"

namespace fq::cli {
// Forward declare to avoid exposing cxxopts in headers
void printGlobalHelp(const std::map<std::string, CommandPtr>& commands);
}  // namespace fq::cli

/**
 * @brief FastQTools 主函数，命令行程序入口
 * @details 负责初始化日志、计时器，解析参数并分发到具体子命令。
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 * @return 程序执行状态码，0 表示成功，非0表示异常
 */
auto main(int argc, char* argv[]) -> int {
    // 解析全局选项（--verbose, --quiet, --log-level）
    std::string logLevel = "info";

    // 构建子命令参数列表（过滤掉全局选项）
    std::vector<char*> subArgs;
    subArgs.push_back(argv[0]);  // 保留程序名

    std::string subcommand;
    bool foundSubcommand = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            logLevel = "debug";
        } else if (arg == "--quiet" || arg == "-q") {
            logLevel = "error";
        } else if (arg.starts_with("--log-level=")) {
            logLevel = arg.substr(12);
        } else if (arg == "--help" || arg == "-h") {
            // 全局帮助
            if (!foundSubcommand) {
                // 注册支持的子命令
                std::map<std::string, fq::cli::CommandPtr> commands;
                commands["stat"] = std::make_unique<fq::cli::commands::StatCommand>();
                commands["filter"] = std::make_unique<fq::cli::commands::FilterCommand>();
                fq::cli::printGlobalHelp(commands);
                return 0;
            }
            subArgs.push_back(argv[i]);
        } else {
            if (!foundSubcommand && arg[0] != '-') {
                subcommand = arg;
                foundSubcommand = true;
            }
            subArgs.push_back(argv[i]);
        }
    }

    // 初始化日志
    if (logLevel == "debug") {
        fq::common::Logger::instance().setLevel(fq::common::Logger::Level::Debug);
    } else if (logLevel == "error") {
        fq::common::Logger::instance().setLevel(fq::common::Logger::Level::Error);
    } else {
        fq::common::Logger::instance().setLevel(fq::common::Logger::Level::Info);
    }

    // 打印项目 Logo
    fq::common::printLogo();

    // 启动主计时器
    fq::common::Timer mainTimer("FastQTools");

    // 注册支持的子命令
    std::map<std::string, fq::cli::CommandPtr> commands;
    commands["stat"] = std::make_unique<fq::cli::commands::StatCommand>();
    commands["filter"] = std::make_unique<fq::cli::commands::FilterCommand>();

    // 检查是否有子命令
    if (!foundSubcommand) {
        fq::cli::printGlobalHelp(commands);
        return 1;
    }

    // 查找对应子命令
    auto commandIt = commands.find(subcommand);
    if (commandIt == commands.end()) {
        spdlog::error("Unknown subcommand: {}", subcommand);
        fq::cli::printGlobalHelp(commands);
        return 1;
    }

    try {
        // 执行子命令
        int subArgc = static_cast<int>(subArgs.size());
        return commandIt->second->execute(subArgc, subArgs.data());
    } catch (const std::exception& e) {
        spdlog::error("An error occurred: {}", e.what());
        return 1;
    }
}

namespace fq::cli {
/**
 * @brief 输出全局帮助信息
 * @details 打印所有可用子命令及其描述。
 * @param commands 命令名称与命令对象的映射表
 */
auto printGlobalHelp(const std::map<std::string, CommandPtr>& commands) -> void {
    std::cout << "Usage: FastQTools [global-options] <command> [options]\n\n"
              << "Global options:\n"
              << "  -v, --verbose        Enable verbose/debug output\n"
              << "  -q, --quiet          Suppress non-error output\n"
              << "  --log-level=LEVEL    Set log level (trace,debug,info,warn,error)\n\n"
              << "Available commands:\n";
    for (const auto& [name, command] : commands) {
        std::cout << "  " << name << "\t\t" << command->getDescription() << "\n";
    }
    std::cout
        << "\nRun 'FastQTools <command> --help' for more information on a specific command.\n";
}
}  // namespace fq::cli
