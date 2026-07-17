/**
 * @file filter_command.cpp
 * @brief filter 子命令实现
 * @details 过滤和修剪 FASTQ 文件
 *
 * @author LessUp
 * @date 2023-10-05
 * @version 2.0
 * @copyright (c) 2023 LessUp. All rights reserved.
 */

#include "filter_command.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "common_options.h"
#include "filter_plan.h"
#include <cxxopts.hpp>

#include <fqtools/fq.h>  // 公共 API Façade（包含 pipeline 接口、predicates、mutators）
#include <fqtools/logging.h>

namespace fq::cli::commands {

FilterCommand::FilterCommand() = default;

FilterCommand::~FilterCommand() = default;

auto FilterCommand::execute(int argc, char* argv[]) -> int {
    cxxopts::Options options(getName(), getDescription());

    // 1. 添加共享参数
    CommonCliOptions::addOptions(options);

    // 2. 添加 filter 特有参数
    addFilterPlanOptions(options);
    options.add_options()("h,help", "Print usage");

    if (argc == 1) {
        std::cout << options.help() << '\n';
        return 0;
    }

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << '\n';
        return 0;
    }

    // 3. 解析共享参数
    auto common = CommonCliOptions::parse(result);
    if (common.inputPath.empty() || common.outputPath.empty()) {
        throw std::invalid_argument(
            "both --input and --output options are required for the filter command");
    }

    auto plan = buildFilterPlan(result, common);
    plan.applyTo(pipeline_);

    auto stats = pipeline_.run();
    if (fq::logging::getLevel() < fq::logging::Level::Error) {
        std::cout << stats.toString() << '\n';
    }

    return 0;
}

auto FilterCommand::getName() const -> std::string {
    return "filter";
}

auto FilterCommand::getDescription() const -> std::string {
    return "Filter and trim FastQ files";
}

}  // namespace fq::cli::commands
