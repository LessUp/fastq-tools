/**
 * @file main.cpp
 * @brief FastQTools 命令行主程序入口
 * @details 负责解析命令行参数，分发子命令，并输出帮助信息。
 * @author FastQTools Team
 * @date 2025-08-01
 * @version 2.0
 * @copyright Copyright (c) 2025 FastQTools
 */

#include <iostream>
#include <string>
#include <vector>

#include "command_registry.h"

#include <fqtools/fq.h>
#include <fqtools/logging.h>

namespace {

auto parseGlobalOptions(int argc,
                        char* argv[],
                        std::vector<char*>& subArgs) -> std::pair<std::string, std::string> {
    std::string logLevel = "info";
    std::string subcommand;
    bool foundSubcommand = false;

    subArgs.push_back(argv[0]);  // 保留程序名

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            logLevel = "debug";
        } else if (arg == "--quiet" || arg == "-q") {
            logLevel = "error";
        } else if (arg.starts_with("--log-level=")) {
            logLevel = arg.substr(12);
        } else if (arg == "--help" || arg == "-h") {
            subArgs.push_back(argv[i]);
        } else {
            if (!foundSubcommand && arg[0] != '-') {
                subcommand = arg;
                foundSubcommand = true;
            }
            subArgs.push_back(argv[i]);
        }
    }

    return {logLevel, subcommand};
}

}  // namespace

auto main(int argc, char* argv[]) -> int {
    std::vector<char*> subArgs;
    auto [logLevel, subcommand] = parseGlobalOptions(argc, argv, subArgs);

    bool quietMode = (logLevel == "error");

    // 初始化日志
    fq::logging::setLevel(logLevel);

    // 打印项目 Logo
    if (!quietMode) {
        fq::common::printLogo();
    }

    // 启动主计时器
    fq::common::Timer mainTimer("FastQTools");

    // 创建命令注册表
    auto registry = fq::cli::createDefaultCommandRegistry();

    // 检查是否有 --help 标志
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--help" || arg == "-h") && subcommand.empty()) {
            registry->printHelp(std::cout);
            return 0;
        }
    }

    // 检查是否有子命令
    if (subcommand.empty()) {
        registry->printHelp(std::cout);
        return 1;
    }

    // 查找并执行子命令
    if (!registry->hasCommand(subcommand)) {
        fq::logging::error("Unknown subcommand: {}", subcommand);
        registry->printHelp(std::cout);
        return 1;
    }

    try {
        int subArgc = static_cast<int>(subArgs.size());
        return registry->executeCommand(subcommand, subArgc, subArgs.data());
    } catch (const std::exception& e) {
        fq::logging::error("An error occurred: {}", e.what());
        return 1;
    }
}
