/**
 * @file common_options.cpp
 * @brief CLI 共享参数实现
 * @details 实现共享参数的解析和转换逻辑
 *
 * @author LessUp
 * @date 2026-05-08
 * @version 2.0
 * @copyright (c) 2026 LessUp. All rights reserved.
 */

#include "common_options.h"

#include <stdexcept>

#include <cxxopts.hpp>

namespace fq::cli {

void CommonCliOptions::addOptions(cxxopts::Options& options) {
    options.add_options()("i,input", "Input FASTQ file", cxxopts::value<std::string>())(
        "o,output", "Output file", cxxopts::value<std::string>())(
        "t,threads",
        "Number of threads (default: 1)",
        cxxopts::value<size_t>()->default_value("1"))(
        "batch-size",
        "Batch size (reads per batch, default: 10000)",
        cxxopts::value<size_t>()->default_value("10000"))(
        "profile",
        "Performance profile: default|lowMemory|highThroughput",
        cxxopts::value<std::string>()->default_value("default"))(
        "memory-limit-gb",
        "Memory limit in GB (0=unlimited)",
        cxxopts::value<size_t>()->default_value("0"));
}

auto CommonCliOptions::parse(const cxxopts::ParseResult& result) -> CommonCliOptions {
    CommonCliOptions opts;

    // 必需参数
    if (result.count("input")) {
        opts.inputPath = result["input"].as<std::string>();
    }
    if (result.count("output")) {
        opts.outputPath = result["output"].as<std::string>();
    }

    // 可选参数
    opts.threadCount = result["threads"].as<size_t>();
    opts.batchSize = result["batch-size"].as<size_t>();

    // 解析性能预设
    std::string profileStr = result["profile"].as<std::string>();
    opts.profile = parseProcessingProfile(profileStr);

    // 内存限制
    size_t memGb = result["memory-limit-gb"].as<size_t>();
    if (memGb > 0) {
        opts.memoryLimitGb = memGb;
    }

    return opts;
}

auto CommonCliOptions::toProcessingOptions() const -> processing::ProcessingOptions {
    processing::ProcessingOptions opts;
    opts.batchSize = batchSize;
    opts.threadCount = threadCount;
    opts.profile = profile;

    if (memoryLimitGb.has_value()) {
        opts.memoryLimitBytes = memoryLimitGb.value() * 1024ULL * 1024ULL * 1024ULL;
    }

    return opts;
}

auto parseProcessingProfile(const std::string& value) -> processing::ProcessingProfile {
    if (value == "default") {
        return processing::ProcessingProfile::Default;
    }
    if (value == "lowMemory") {
        return processing::ProcessingProfile::LowMemory;
    }
    if (value == "highThroughput") {
        return processing::ProcessingProfile::HighThroughput;
    }

    throw std::invalid_argument("Invalid profile: " + value +
                                ". Valid options: default, lowMemory, highThroughput");
}

}  // namespace fq::cli
