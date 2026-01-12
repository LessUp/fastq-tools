#include <benchmark/benchmark.h>
#include <fqtools/io/fastq_reader.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

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

struct BasicStats {
    std::size_t total_reads = 0;
    std::size_t total_bases = 0;
    std::size_t min_length = std::numeric_limits<std::size_t>::max();
    std::size_t max_length = 0;
    double mean_quality = 0.0;
    std::array<std::size_t, 5> base_counts = {0, 0, 0, 0, 0};  // A, T, G, C, N
};

BasicStats computeBasicStats(fq::io::FastqReader& reader) {
    BasicStats stats;
    fq::io::FastqBatch batch;
    double quality_sum = 0.0;
    std::size_t quality_count = 0;

    while (reader.nextBatch(batch)) {
        for (const auto& rec : batch) {
            ++stats.total_reads;
            stats.total_bases += rec.seq.size();
            
            if (rec.seq.size() < stats.min_length) {
                stats.min_length = rec.seq.size();
            }
            if (rec.seq.size() > stats.max_length) {
                stats.max_length = rec.seq.size();
            }

            for (char c : rec.seq) {
                switch (c) {
                    case 'A': case 'a': ++stats.base_counts[0]; break;
                    case 'T': case 't': ++stats.base_counts[1]; break;
                    case 'G': case 'g': ++stats.base_counts[2]; break;
                    case 'C': case 'c': ++stats.base_counts[3]; break;
                    default: ++stats.base_counts[4]; break;
                }
            }

            for (char c : rec.qual) {
                quality_sum += static_cast<double>(c - 33);
                ++quality_count;
            }
        }
    }

    if (quality_count > 0) {
        stats.mean_quality = quality_sum / static_cast<double>(quality_count);
    }
    if (stats.min_length == std::numeric_limits<std::size_t>::max()) {
        stats.min_length = 0;
    }

    return stats;
}

}  // namespace

// Stat: 基本统计 (reads数, bases数, 长度范围)
static void BM_Stat_Basic(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("stat_basic_input", test_data);
    const auto file_size = std::filesystem::file_size(input_path);

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        BasicStats stats = computeBasicStats(reader);
        ::benchmark::DoNotOptimize(stats.total_reads);
        ::benchmark::DoNotOptimize(stats.total_bases);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);
    state.counters["throughput_MB_s"] =
        ::benchmark::Counter(static_cast<double>(file_size), ::benchmark::Counter::kIsRate,
                             ::benchmark::Counter::kIs1024);

    std::filesystem::remove(input_path);
}

// Stat: 碱基组成统计
static void BM_Stat_BaseComposition(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("stat_base_input", test_data);
    const auto file_size = std::filesystem::file_size(input_path);

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        fq::io::FastqBatch batch;
        std::array<std::size_t, 5> base_counts = {0, 0, 0, 0, 0};

        while (reader.nextBatch(batch)) {
            for (const auto& rec : batch) {
                for (char c : rec.seq) {
                    switch (c) {
                        case 'A': case 'a': ++base_counts[0]; break;
                        case 'T': case 't': ++base_counts[1]; break;
                        case 'G': case 'g': ++base_counts[2]; break;
                        case 'C': case 'c': ++base_counts[3]; break;
                        default: ++base_counts[4]; break;
                    }
                }
            }
        }
        ::benchmark::DoNotOptimize(base_counts);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(input_path);
}

// Stat: 质量分布统计
static void BM_Stat_QualityDistribution(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("stat_qual_input", test_data);
    const auto file_size = std::filesystem::file_size(input_path);

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        fq::io::FastqBatch batch;
        std::array<std::size_t, 42> qual_histogram = {};  // Q0-Q41

        while (reader.nextBatch(batch)) {
            for (const auto& rec : batch) {
                for (char c : rec.qual) {
                    int q = static_cast<int>(c) - 33;
                    if (q >= 0 && q < 42) {
                        ++qual_histogram[static_cast<std::size_t>(q)];
                    }
                }
            }
        }
        ::benchmark::DoNotOptimize(qual_histogram);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(input_path);
}

// Stat: 长度分布统计
static void BM_Stat_LengthDistribution(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("stat_len_input", test_data);
    const auto file_size = std::filesystem::file_size(input_path);

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        fq::io::FastqBatch batch;
        std::vector<std::size_t> length_histogram(500, 0);

        while (reader.nextBatch(batch)) {
            for (const auto& rec : batch) {
                std::size_t len = rec.seq.size();
                if (len < length_histogram.size()) {
                    ++length_histogram[len];
                }
            }
        }
        ::benchmark::DoNotOptimize(length_histogram);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(input_path);
}

// Stat: 完整统计 (所有指标)
static void BM_Stat_Full(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    std::string test_data = generateFastQFile(num_reads);
    std::filesystem::path input_path = write_temp_fastq("stat_full_input", test_data);
    const auto file_size = std::filesystem::file_size(input_path);

    for (auto _ : state) {
        fq::io::FastqReader reader(input_path.string());
        BasicStats stats = computeBasicStats(reader);
        ::benchmark::DoNotOptimize(stats);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);
    state.counters["throughput_MB_s"] =
        ::benchmark::Counter(static_cast<double>(file_size), ::benchmark::Counter::kIsRate,
                             ::benchmark::Counter::kIs1024);

    std::filesystem::remove(input_path);
}

BENCHMARK(BM_Stat_Basic)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Stat_BaseComposition)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Stat_QualityDistribution)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Stat_LengthDistribution)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_Stat_Full)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

}  // namespace fq::benchmark
