#include <benchmark/benchmark.h>
#include <fqtools/io/fastq_reader.h>
#include <fqtools/io/fastq_writer.h>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

namespace fq::benchmark {

namespace {

std::string generateFastQFile(std::size_t num_reads, std::size_t read_length = 150) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> base_dis(0, 4);
    static std::uniform_int_distribution<> qual_dis(20, 40);

    static const char bases[] = "ATGCN";

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

double calculateMeanQuality(const std::string& qual) {
    if (qual.empty()) return 0.0;
    double sum = 0.0;
    for (char c : qual) {
        sum += static_cast<double>(c - 33);
    }
    return sum / static_cast<double>(qual.size());
}

double calculateNRatio(const std::string& seq) {
    if (seq.empty()) return 0.0;
    std::size_t n_count = 0;
    for (char c : seq) {
        if (c == 'N' || c == 'n') ++n_count;
    }
    return static_cast<double>(n_count) / static_cast<double>(seq.size());
}

}  // namespace

// Filter: 无过滤条件 (baseline)
static void BM_Filter_NoFilter(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("filter_input", test_data);
    std::filesystem::path output_path = std::filesystem::temp_directory_path() / "filter_output.fastq";

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        fq::io::FastqWriter writer(output_path.string());
        fq::io::FastqBatch batch;
        std::size_t passed = 0;
        while (reader.next_batch(batch)) {
            for (const auto& rec : batch) {
                writer.write(rec);
                ++passed;
            }
        }
        ::benchmark::DoNotOptimize(passed);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

// Filter: 最小长度过滤
static void BM_Filter_MinLength(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    const std::size_t min_length = 100;
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("filter_minlen_input", test_data);
    std::filesystem::path output_path = std::filesystem::temp_directory_path() / "filter_minlen_output.fastq";

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        fq::io::FastqWriter writer(output_path.string());
        fq::io::FastqBatch batch;
        std::size_t passed = 0;
        while (reader.next_batch(batch)) {
            for (const auto& rec : batch) {
                if (rec.seq.size() >= min_length) {
                    writer.write(rec);
                    ++passed;
                }
            }
        }
        ::benchmark::DoNotOptimize(passed);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

// Filter: 最小质量过滤
static void BM_Filter_MinQuality(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    const double min_quality = 25.0;
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("filter_minqual_input", test_data);
    std::filesystem::path output_path = std::filesystem::temp_directory_path() / "filter_minqual_output.fastq";

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        fq::io::FastqWriter writer(output_path.string());
        fq::io::FastqBatch batch;
        std::size_t passed = 0;
        while (reader.next_batch(batch)) {
            for (const auto& rec : batch) {
                if (calculateMeanQuality(rec.qual) >= min_quality) {
                    writer.write(rec);
                    ++passed;
                }
            }
        }
        ::benchmark::DoNotOptimize(passed);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

// Filter: N 比例过滤
static void BM_Filter_MaxNRatio(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    const double max_n_ratio = 0.1;
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("filter_nratio_input", test_data);
    std::filesystem::path output_path = std::filesystem::temp_directory_path() / "filter_nratio_output.fastq";

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        fq::io::FastqWriter writer(output_path.string());
        fq::io::FastqBatch batch;
        std::size_t passed = 0;
        while (reader.next_batch(batch)) {
            for (const auto& rec : batch) {
                if (calculateNRatio(rec.seq) <= max_n_ratio) {
                    writer.write(rec);
                    ++passed;
                }
            }
        }
        ::benchmark::DoNotOptimize(passed);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

// Filter: 组合过滤 (长度 + 质量 + N比例)
static void BM_Filter_Combined(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    const std::size_t min_length = 100;
    const double min_quality = 25.0;
    const double max_n_ratio = 0.1;
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("filter_combined_input", test_data);
    std::filesystem::path output_path = std::filesystem::temp_directory_path() / "filter_combined_output.fastq";

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        fq::io::FastqWriter writer(output_path.string());
        fq::io::FastqBatch batch;
        std::size_t passed = 0;
        while (reader.next_batch(batch)) {
            for (const auto& rec : batch) {
                if (rec.seq.size() >= min_length &&
                    calculateMeanQuality(rec.qual) >= min_quality &&
                    calculateNRatio(rec.seq) <= max_n_ratio) {
                    writer.write(rec);
                    ++passed;
                }
            }
        }
        ::benchmark::DoNotOptimize(passed);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(input_path);
    std::filesystem::remove(output_path);
}

BENCHMARK(BM_Filter_NoFilter)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Filter_MinLength)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Filter_MinQuality)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Filter_MaxNRatio)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Filter_Combined)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

}  // namespace fq::benchmark
