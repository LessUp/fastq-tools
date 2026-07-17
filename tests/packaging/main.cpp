#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

#include <fqtools/fq.h>

auto main() -> int {
    static_assert(!std::is_copy_constructible_v<fq::processing::Pipeline>);
    static_assert(!std::is_copy_constructible_v<fq::statistics::Calculator>);
    const auto stem = std::string("fastqtools-package-") +
        std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    const auto root = std::filesystem::temp_directory_path() / stem;
    const auto input = root.string() + ".fastq";
    const auto filtered = root.string() + ".filtered.fastq";
    const auto report = root.string() + ".stats.txt";

    std::ofstream(input) << "@consumer-read\nACGT\n+\nIIII\n";

    fq::processing::Pipeline pipeline;
    pipeline.setInputPath(input);
    pipeline.setOutputPath(filtered);
    fq::processing::ProcessingOptions processing;
    processing.batchSize = 1;
    processing.threadCount = 1;
    pipeline.setProcessingOptions(processing);
    const auto filterStats = pipeline.run();

    fq::statistics::StatisticOptions statistics;
    statistics.inputFastqPath = filtered;
    statistics.outputStatPath = report;
    statistics.processing.batchSize = 1;
    statistics.processing.threadCount = 1;
    fq::statistics::Calculator calculator(std::move(statistics));
    calculator.run();

    const bool successful = filterStats.totalReads == 1 && filterStats.passedReads == 1 &&
        std::filesystem::exists(report);
    std::filesystem::remove(input);
    std::filesystem::remove(filtered);
    std::filesystem::remove(report);
    if (!successful) {
        return 1;
    }
    std::cout << "FastQTools\n";
    return 0;
}
