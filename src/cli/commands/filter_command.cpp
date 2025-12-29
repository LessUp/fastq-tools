#include "filter_command.h"

#include <iomanip>
#include <iostream>

#include <cxxopts.hpp>

#include <fqtools/fq.h>  // 公共 API Façade（包含 pipeline 接口、predicates、mutators）

namespace fq::cli::commands {

struct FilterCommand::Config {
    std::string inputFile;
    std::string outputFile;
    size_t threadCount = 1;
    size_t batchSize = 10000;
    size_t readChunkBytes = 1048576;
    size_t batchCapacityBytes = 4194304;
    size_t zlibBufferBytes = 131072;
    size_t writerBufferBytes = 131072;
    size_t maxInFlightBatches = 0;
    size_t memoryLimitGb = 10;
};

// Use the factory in the constructor
FilterCommand::FilterCommand()
    : config_(std::make_unique<Config>()),
      pipeline_(fq::processing::createProcessingPipeline()) {}

FilterCommand::~FilterCommand() = default;

auto FilterCommand::execute(int argc, char* argv[]) -> int {
    cxxopts::Options options(getName(), getDescription());
    options.add_options()("i,input", "Input FASTQ file (required)", cxxopts::value<std::string>())(
        "o,output", "Output FASTQ file (required)", cxxopts::value<std::string>())(
        "t,threads", "Number of threads", cxxopts::value<size_t>()->default_value("1"))(
        "batch-size",
        "Batch size (reads per batch)",
        cxxopts::value<size_t>()->default_value("10000"))(
        "read-chunk-bytes",
        "Reader chunk size in bytes",
        cxxopts::value<size_t>()->default_value("1048576"))(
        "batch-capacity-bytes",
        "Batch buffer capacity in bytes",
        cxxopts::value<size_t>()->default_value("4194304"))(
        "zlib-buffer-bytes",
        "zlib internal buffer size in bytes",
        cxxopts::value<size_t>()->default_value("131072"))(
        "writer-buffer-bytes",
        "Writer buffer size in bytes",
        cxxopts::value<size_t>()->default_value("131072"))(
        "in-flight",
        "Max in-flight batches (0=auto)",
        cxxopts::value<size_t>()->default_value("0"))(
        "memory-limit-gb",
        "Memory limit (GB) for in-flight batches (0=unlimited)",
        cxxopts::value<size_t>()->default_value("10"))("quality-encoding",
                                                       "Quality encoding offset (33 or 64)",
                                                       cxxopts::value<int>()->default_value("33"))(
        "min-quality", "Minimum average quality threshold", cxxopts::value<double>())(
        "min-length", "Minimum read length", cxxopts::value<size_t>())(
        "max-length", "Maximum read length", cxxopts::value<size_t>())(
        "max-n-ratio", "Maximum N ratio (0.0-1.0)", cxxopts::value<double>())(
        "trim-quality", "Trim bases below quality threshold", cxxopts::value<double>())(
        "trim-mode",
        "Trim mode (both,five,three)",
        cxxopts::value<std::string>()->default_value("both"))("h,help", "Print usage");

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
        std::cerr << "Error: both --input and --output options are required for the filter command."
                  << std::endl;
        std::cerr << options.help() << std::endl;
        return 1;
    }

    config_->inputFile = result["input"].as<std::string>();
    config_->outputFile = result["output"].as<std::string>();

    config_->threadCount = result["threads"].as<size_t>();

    pipeline_->setInputPath(config_->inputFile);
    pipeline_->setOutputPath(config_->outputFile);

    // Use the config from the interface
    fq::processing::ProcessingConfig pipelineConfig;
    pipelineConfig.threadCount = config_->threadCount;
    pipelineConfig.batchSize = result["batch-size"].as<size_t>();
    pipelineConfig.readChunkBytes = result["read-chunk-bytes"].as<size_t>();
    pipelineConfig.batchCapacityBytes = result["batch-capacity-bytes"].as<size_t>();
    pipelineConfig.zlibBufferBytes = result["zlib-buffer-bytes"].as<size_t>();
    pipelineConfig.writerBufferBytes = result["writer-buffer-bytes"].as<size_t>();
    pipelineConfig.maxInFlightBatches = result["in-flight"].as<size_t>();
    const size_t memGb = result["memory-limit-gb"].as<size_t>();
    pipelineConfig.memoryLimitBytes =
        memGb == 0 ? 0 : (memGb * 1024ULL * 1024ULL * 1024ULL);
    pipeline_->setProcessingConfig(pipelineConfig);

    // Wire predicates and mutators from CLI options
    const int qualityEncoding = result["quality-encoding"].as<int>();

    if (result.count("min-quality")) {
        double minQ = result["min-quality"].as<double>();
        pipeline_->addReadPredicate(
            std::make_unique<fq::processing::MinQualityPredicate>(minQ, qualityEncoding));
    }

    if (result.count("min-length")) {
        size_t minLen = result["min-length"].as<size_t>();
        pipeline_->addReadPredicate(
            std::make_unique<fq::processing::MinLengthPredicate>(minLen));
    }

    if (result.count("max-length")) {
        size_t maxLen = result["max-length"].as<size_t>();
        pipeline_->addReadPredicate(
            std::make_unique<fq::processing::MaxLengthPredicate>(maxLen));
    }

    if (result.count("max-n-ratio")) {
        double maxN = result["max-n-ratio"].as<double>();
        pipeline_->addReadPredicate(
            std::make_unique<fq::processing::MaxNRatioPredicate>(maxN));
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

    auto stats = pipeline_->run();
    std::cout << stats.toString() << std::endl;

    return 0;
}

auto FilterCommand::getName() const -> std::string {
    return "filter";
}

auto FilterCommand::getDescription() const -> std::string {
    return "Filter and trim FastQ files";
}

}  // namespace fq::cli::commands
