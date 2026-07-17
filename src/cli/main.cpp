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
#include <stdexcept>
#include <string>
#include <vector>

#include "command_registry.h"
#include <cxxopts.hpp>

#include <fqtools/fq.h>
#include <fqtools/logging.h>

namespace {

constexpr int kExitSuccess = 0;
constexpr int kExitRuntimeError = 1;
constexpr int kExitParameterError = 2;
constexpr int kExitFormatError = 3;
constexpr int kExitIoError = 4;

auto parseGlobalOptions(int argc, char* argv[], std::vector<char*>& subArgs)
    -> std::pair<std::string, std::string> {
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
    try {
        std::vector<char*> subArgs;
        auto [logLevel, subcommand] = parseGlobalOptions(argc, argv, subArgs);

        const bool quietMode = (logLevel == "error");

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
                return kExitSuccess;
            }
        }

        // 检查是否有子命令
        if (subcommand.empty()) {
            registry->printHelp(std::cout);
            throw std::invalid_argument("a subcommand is required");
        }

        // 查找并执行子命令
        if (!registry->hasCommand(subcommand)) {
            registry->printHelp(std::cout);
            throw std::invalid_argument("Unknown subcommand: " + subcommand);
        }

        int subArgc = static_cast<int>(subArgs.size());
        return registry->executeCommand(subcommand, subArgc, subArgs.data());
    } catch (const fq::error::FastQException& e) {
        fq::logging::error("{}", e.what());
        switch (e.category()) {
            case fq::error::ErrorCategory::IO:
                return kExitIoError;
            case fq::error::ErrorCategory::Format:
                return kExitFormatError;
            case fq::error::ErrorCategory::Configuration:
            case fq::error::ErrorCategory::Validation:
                return kExitParameterError;
            case fq::error::ErrorCategory::Processing:
            case fq::error::ErrorCategory::Resource:
                return kExitRuntimeError;
        }
        return kExitRuntimeError;
    } catch (const cxxopts::exceptions::exception& e) {
        fq::logging::error("Parameter error: {}", e.what());
        return kExitParameterError;
    } catch (const std::invalid_argument& e) {
        fq::logging::error("Parameter error: {}", e.what());
        return kExitParameterError;
    } catch (const std::out_of_range& e) {
        fq::logging::error("Parameter error: {}", e.what());
        return kExitParameterError;
    } catch (const std::exception& e) {
        fq::logging::error("Unhandled error: {}", e.what());
        return kExitRuntimeError;
    }
}
