/**
 * @file pipeline_benchmark.cpp
 * @brief 处理流水线端到端性能基准测试
 * @details 测试串行和并行处理流水线的实际吞吐量
 *
 * @author FastQTools Team
 * @date 2025
 * @copyright Copyright (c) 2025 FastQTools
 */

#include <benchmark/benchmark.h>
#include <fqtools/processing/processing_pipeline_interface.h>
#include <fqtools/processing/mutators.h>
#include <fqtools/processing/predicates.h>

#include <filesystem>
#include <fstream>
#include <random>
#include <sstream>
#include <string>

namespace fq::benchmark {

namespace {

std::string generateFastQFile(std::size_t num_reads, std::size_t read_length = 150) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> base_dis(0, 3);
    static std::uniform_int_distribution<> qual_dis(20, 40);

    static const char bases[] = "ATGC";

    std::ostringstream oss;
    for (std::size_t i = 0; i < num_reads; ++i) {
        oss << "@read_" << i << "\n";
        for (std::size_t j = 0; j < read_length; ++j) {
            oss << bases[base_dis(gen)];
        }
        oss << "\n+\n";
        for (std::size_t j = 0; j < read_length; ++j) {
            oss << static_cast<char>(qual_dis(gen) + 33);
        }
        oss << "\n";
    }
    return oss.str();
}

std::filesystem::path write_temp_fastq(const std::string& prefix, const std::string& content) {
    namespace fs = std::filesystem;
    fs::path path = fs::temp_directory_path() / (prefix + ".fastq");
    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    ofs << content;
    ofs.close();
    return path;
}

}  // namespace

// 串行处理流水线 benchmark
static void BM_Pipeline_Sequential(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("pipeline_seq_input", test_data);
    std::filesystem::path output_path =
        std::filesystem::temp_directory_path() / "pipeline_seq_output.fastq";
    const auto file_size = std::filesystem::file_size(input_path);

    for (auto _ : state) {
        auto pipeline = fq::processing::createProcessingPipeline();

        fq::processing::ProcessingConfig config;
        config.threadCount = 1;  // 串行模式
        config.batchSize = 10000;

        pipeline->setInputPath(input_path.string());
        pipeline->setOutputPath(output_path.string());
        pipeline->setProcessingConfig(config);

        auto stats = pipeline->run();
        ::benchmark::DoNotOptimize(stats.totalReads);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

// 并行处理流水线 benchmark (2 线程)
static void BM_Pipeline_Parallel_2T(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("pipeline_par2_input", test_data);
    std::filesystem::path output_path =
        std::filesystem::temp_directory_path() / "pipeline_par2_output.fastq";
    const auto file_size = std::filesystem::file_size(input_path);

    for (auto _ : state) {
        auto pipeline = fq::processing::createProcessingPipeline();

        fq::processing::ProcessingConfig config;
        config.threadCount = 2;  // 2 线程
        config.batchSize = 10000;

        pipeline->setInputPath(input_path.string());
        pipeline->setOutputPath(output_path.string());
        pipeline->setProcessingConfig(config);

        auto stats = pipeline->run();
        ::benchmark::DoNotOptimize(stats.totalReads);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);
    state.counters["threads"] = 2;

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

// 并行处理流水线 benchmark (4 线程)
static void BM_Pipeline_Parallel_4T(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("pipeline_par4_input", test_data);
    std::filesystem::path output_path =
        std::filesystem::temp_directory_path() / "pipeline_par4_output.fastq";
    const auto file_size = std::filesystem::file_size(input_path);

    for (auto _ : state) {
        auto pipeline = fq::processing::createProcessingPipeline();

        fq::processing::ProcessingConfig config;
        config.threadCount = 4;  // 4 线程
        config.batchSize = 10000;

        pipeline->setInputPath(input_path.string());
        pipeline->setOutputPath(output_path.string());
        pipeline->setProcessingConfig(config);

        auto stats = pipeline->run();
        ::benchmark::DoNotOptimize(stats.totalReads);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);
    state.counters["threads"] = 4;

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

// 并行处理流水线 benchmark (8 线程)
static void BM_Pipeline_Parallel_8T(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("pipeline_par8_input", test_data);
    std::filesystem::path output_path =
        std::filesystem::temp_directory_path() / "pipeline_par8_output.fastq";
    const auto file_size = std::filesystem::file_size(input_path);

    for (auto _ : state) {
        auto pipeline = fq::processing::createProcessingPipeline();

        fq::processing::ProcessingConfig config;
        config.threadCount = 8;  // 8 线程
        config.batchSize = 10000;

        pipeline->setInputPath(input_path.string());
        pipeline->setOutputPath(output_path.string());
        pipeline->setProcessingConfig(config);

        auto stats = pipeline->run();
        ::benchmark::DoNotOptimize(stats.totalReads);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);
    state.counters["threads"] = 8;

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

// 注册 benchmark
BENCHMARK(BM_Pipeline_Sequential)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Pipeline_Parallel_2T)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Pipeline_Parallel_4T)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Pipeline_Parallel_8T)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

}  // namespace fq::benchmark
