#include "stat_command.h"

#include <iostream>

#include <cxxopts.hpp>

#include <fqtools/statistics/statistic_calculator.h>

namespace fq::cli::commands {

auto StatCommand::execute(int argc, char* argv[]) -> int {
    cxxopts::Options options(getName(), getDescription());
    options.add_options()("i,input", "Input FASTQ file", cxxopts::value<std::string>())(
        "o,output", "Output statistics file", cxxopts::value<std::string>())(
        "t,threads", "Number of threads", cxxopts::value<size_t>()->default_value("1"))(
        "batch-size",
        "Batch size (reads per batch)",
        cxxopts::value<size_t>()->default_value("50000"))(
        "read-chunk-bytes",
        "Reader chunk size in bytes",
        cxxopts::value<size_t>()->default_value("1048576"))(
        "batch-capacity-bytes",
        "Batch buffer capacity in bytes",
        cxxopts::value<size_t>()->default_value("4194304"))(
        "zlib-buffer-bytes",
        "zlib internal buffer size in bytes",
        cxxopts::value<size_t>()->default_value("131072"))(
        "in-flight",
        "Max in-flight batches (0=auto)",
        cxxopts::value<size_t>()->default_value("0"))(
        "memory-limit-gb",
        "Memory limit (GB) for in-flight batches (0=unlimited)",
        cxxopts::value<size_t>()->default_value("10"))("h,help", "Print usage");

    if (argc == 1) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (!result.count("input") || !result.count("output")) {
        std::cerr << "Error: both --input and --output options are required for the stat command."
                  << std::endl;
        std::cerr << options.help() << std::endl;
        return 1;
    }

    // Use the interface-level options struct
    fq::statistic::StatisticOptions statOptions;
    statOptions.inputFastqPath = result["input"].as<std::string>();
    statOptions.outputStatPath = result["output"].as<std::string>();
    statOptions.threadCount = static_cast<uint32_t>(result["threads"].as<size_t>());
    statOptions.batchSize = static_cast<uint32_t>(result["batch-size"].as<size_t>());
    statOptions.readChunkBytes = result["read-chunk-bytes"].as<size_t>();
    statOptions.batchCapacityBytes = result["batch-capacity-bytes"].as<size_t>();
    statOptions.zlibBufferBytes = result["zlib-buffer-bytes"].as<size_t>();
    statOptions.maxInFlightBatches = result["in-flight"].as<size_t>();
    const size_t memGb = result["memory-limit-gb"].as<size_t>();
    statOptions.memoryLimitBytes = memGb == 0 ? 0 : (memGb * 1024ULL * 1024ULL * 1024ULL);

    try {
        // Use the factory to create an instance of the calculator
        auto stater = fq::statistic::createStatisticCalculator(statOptions);

        // Call run via the interface pointer
        stater->run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

auto StatCommand::getName() const -> std::string {
    return "stat";
}

auto StatCommand::getDescription() const -> std::string {
    return "Generate statistics for a FASTQ file";
}

}  // namespace fq::cli::commands
