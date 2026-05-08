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

#include "common_options.h"

#include <iomanip>
#include <iostream>

#include <cxxopts.hpp>

#include <fqtools/fq.h>  // 公共 API Façade（包含 pipeline 接口、predicates、mutators）
#include <fqtools/logging.h>
#include <spdlog/spdlog.h>

namespace fq::cli::commands {

// FilterCommand 内部配置仅保存输入输出路径
// 其他配置参数直接使用 fq::processing::ProcessingOptions
struct FilterCommand::Config {
    std::string inputFile;
    std::string outputFile;
};

// Use the factory in the constructor
FilterCommand::FilterCommand()
    : config_(std::make_unique<Config>()), pipeline_(fq::processing::createProcessingPipeline()) {}

FilterCommand::~FilterCommand() = default;

auto FilterCommand::execute(int argc, char* argv[]) -> int {
    cxxopts::Options options(getName(), getDescription());

    // 1. 添加共享参数
    CommonCliOptions::addOptions(options);

    // 2. 添加 filter 特有参数
    options.add_options()
        ("quality-encoding", "Quality encoding offset (33 or 64)",
         cxxopts::value<int>()->default_value("33"))
        ("min-quality", "Minimum average quality threshold", cxxopts::value<double>())
        ("min-length", "Minimum read length", cxxopts::value<size_t>())
        ("max-length", "Maximum read length", cxxopts::value<size_t>())
        ("max-n-ratio", "Maximum N ratio (0.0-1.0)", cxxopts::value<double>())
        ("trim-quality", "Trim bases below quality threshold", cxxopts::value<double>())
        ("trim-mode", "Trim mode (both,five,three)",
         cxxopts::value<std::string>()->default_value("both"))
        ("adapter-seq", "Trim adapter sequence from 3' end (repeatable)",
         cxxopts::value<std::vector<std::string>>())
        ("adapter-min-overlap", "Minimum adapter overlap",
         cxxopts::value<size_t>()->default_value("3"))
        ("adapter-max-mismatches", "Maximum adapter mismatches",
         cxxopts::value<size_t>()->default_value("1"))
        ("trim-poly-g", "Trim polyG tail with minimum run length", cxxopts::value<size_t>())
        ("trim-poly-x", "Trim low-complexity polyX tail with minimum run length",
         cxxopts::value<size_t>())
        ("h,help", "Print usage");

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
        fq::logging::error(
            "Error: both --input and --output options are required for the filter command.");
        std::cout << options.help() << '\n';
        return 1;
    }

    config_->inputFile = common.inputPath;
    config_->outputFile = common.outputPath;

    pipeline_->setInputPath(config_->inputFile);
    pipeline_->setOutputPath(config_->outputFile);

    // 4. 设置处理选项
    pipeline_->setProcessingOptions(common.toProcessingOptions());

    // 5. Wire predicates and mutators from CLI options
    const int qualityEncoding = result["quality-encoding"].as<int>();

    if (result.count("min-quality")) {
        double minQ = result["min-quality"].as<double>();
        pipeline_->addReadPredicate(
            std::make_unique<fq::processing::MinQualityPredicate>(minQ, qualityEncoding));
    }

    if (result.count("min-length")) {
        size_t minLen = result["min-length"].as<size_t>();
        pipeline_->addReadPredicate(std::make_unique<fq::processing::MinLengthPredicate>(minLen));
    }

    if (result.count("max-length")) {
        size_t maxLen = result["max-length"].as<size_t>();
        pipeline_->addReadPredicate(std::make_unique<fq::processing::MaxLengthPredicate>(maxLen));
    }

    if (result.count("max-n-ratio")) {
        double maxN = result["max-n-ratio"].as<double>();
        pipeline_->addReadPredicate(std::make_unique<fq::processing::MaxNRatioPredicate>(maxN));
    }

    if (result.count("trim-quality")) {
        double trimQ = result["trim-quality"].as<double>();
        std::string modeStr = result["trim-mode"].as<std::string>();
        fq::processing::QualityTrimmer::TrimMode mode =
            fq::processing::QualityTrimmer::TrimMode::Both;
        if (modeStr == "five")
            mode = fq::processing::QualityTrimmer::TrimMode::FivePrime;
        else if (modeStr == "three")
            mode = fq::processing::QualityTrimmer::TrimMode::ThreePrime;
        pipeline_->addReadMutator(std::make_unique<fq::processing::QualityTrimmer>(
            trimQ, /*min_length*/ 1, mode, qualityEncoding));
    }

    if (result.count("adapter-seq")) {
        pipeline_->addReadMutator(std::make_unique<fq::processing::AdapterTrimmer>(
            result["adapter-seq"].as<std::vector<std::string>>(),
            result["adapter-min-overlap"].as<size_t>(),
            result["adapter-max-mismatches"].as<size_t>()));
    }

    if (result.count("trim-poly-g")) {
        pipeline_->addReadMutator(std::make_unique<fq::processing::PolyTailTrimmer>(
            fq::processing::PolyTailTrimmer::TailKind::PolyG, result["trim-poly-g"].as<size_t>()));
    }

    if (result.count("trim-poly-x")) {
        pipeline_->addReadMutator(std::make_unique<fq::processing::PolyTailTrimmer>(
            fq::processing::PolyTailTrimmer::TailKind::PolyX, result["trim-poly-x"].as<size_t>()));
    }

    auto stats = pipeline_->run();
    if (spdlog::get_level() < spdlog::level::err) {
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
