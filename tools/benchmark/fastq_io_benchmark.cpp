#include <benchmark/benchmark.h>
#include <fqtools/io/fastq_reader.h>
#include <fqtools/io/fastq_writer.h>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

namespace fq::benchmark {

class TestDataGenerator {
public:
    static std::string generateFastQRecord(std::size_t read_length = 150) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> base_dis(0, 3);
        static std::uniform_int_distribution<> qual_dis(33, 73);

        static const char bases[] = "ATGC";

        std::ostringstream oss;
        oss << "@read_" << gen() << "\n";

        for (std::size_t i = 0; i < read_length; ++i) {
            oss << bases[base_dis(gen)];
        }
        oss << "\n+\n";

        for (std::size_t i = 0; i < read_length; ++i) {
            oss << static_cast<char>(qual_dis(gen));
        }
        oss << "\n";

        return oss.str();
    }

    static std::string generateFastQFile(std::size_t num_reads, std::size_t read_length = 150) {
        std::ostringstream oss;
        for (std::size_t i = 0; i < num_reads; ++i) {
            oss << generateFastQRecord(read_length);
        }
        return oss.str();
    }
};

namespace {

std::filesystem::path write_temp_fastq(const std::string& prefix, const std::string& content) {
    namespace fs = std::filesystem;
    fs::path path = fs::temp_directory_path() / (prefix + ".fastq");

    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    ofs << content;
    ofs.close();

    return path;
}

}  // namespace

static void BM_FastQReader_Small(::benchmark::State& state) {
    const std::size_t num_reads = 10000;
    const std::size_t read_length = 150;

    std::string test_data = TestDataGenerator::generateFastQFile(num_reads, read_length);
    std::filesystem::path path = write_temp_fastq("benchmark_reader_small", test_data);
    const auto file_size = std::filesystem::file_size(path);

    for (auto _ : state) {
        fq::io::FastqReader reader(path.string());
        fq::io::FastqBatch batch;
        std::size_t total_reads = 0;
        while (reader.next_batch(batch)) {
            total_reads += batch.size();
        }
        ::benchmark::DoNotOptimize(total_reads);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(path);
}

static void BM_FastQReader_Medium(::benchmark::State& state) {
    const std::size_t num_reads = 100000;
    const std::size_t read_length = 150;

    std::string test_data = TestDataGenerator::generateFastQFile(num_reads, read_length);
    std::filesystem::path path = write_temp_fastq("benchmark_reader_medium", test_data);
    const auto file_size = std::filesystem::file_size(path);

    for (auto _ : state) {
        fq::io::FastqReader reader(path.string());
        fq::io::FastqBatch batch;
        std::size_t total_reads = 0;
        while (reader.next_batch(batch)) {
            total_reads += batch.size();
        }
        ::benchmark::DoNotOptimize(total_reads);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(path);
}


static void BM_FastQReader_Parameterized(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    const std::size_t read_length = 150;

    std::string test_data = TestDataGenerator::generateFastQFile(num_reads, read_length);
    std::filesystem::path path = write_temp_fastq("benchmark_reader_param", test_data);
    const auto file_size = std::filesystem::file_size(path);

    for (auto _ : state) {
        fq::io::FastqReader reader(path.string());
        fq::io::FastqBatch batch;
        std::size_t total_reads = 0;
        while (reader.next_batch(batch)) {
            total_reads += batch.size();
        }
        ::benchmark::DoNotOptimize(total_reads);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));
    state.counters["reads"] = static_cast<double>(num_reads);

    std::filesystem::remove(path);
}

static void BM_FastQWriter_Small(::benchmark::State& state) {
    const std::size_t num_reads = 10000;
    const std::size_t read_length = 150;

    struct ReadData {
        std::string id;
        std::string seq;
        std::string qual;
    };

    std::vector<ReadData> reads;
    reads.reserve(num_reads);
    for (std::size_t i = 0; i < num_reads; ++i) {
        ReadData r;
        r.id = "read_" + std::to_string(i);
        r.seq.assign(read_length, 'A');
        r.qual.assign(read_length, 'I');
        reads.push_back(std::move(r));
    }

    namespace fs = std::filesystem;
    fs::path path = fs::temp_directory_path() / "benchmark_writer_small.fastq.gz";

    for (auto _ : state) {
        fq::io::FastqWriter writer(path.string());
        for (const auto& r : reads) {
            fq::io::FastqRecord rec{};
            rec.id = r.id;
            rec.comment = {};
            rec.seq = r.seq;
            rec.qual = r.qual;
            rec.plus = "+";
            writer.write(rec);
        }
        ::benchmark::DoNotOptimize(path.string());
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.counters["reads"] = static_cast<double>(num_reads);

    fs::remove(path);
}

static void BM_FastQWriter_Medium(::benchmark::State& state) {
    const std::size_t num_reads = 100000;
    const std::size_t read_length = 150;

    struct ReadData {
        std::string id;
        std::string seq;
        std::string qual;
    };

    std::vector<ReadData> reads;
    reads.reserve(num_reads);
    for (std::size_t i = 0; i < num_reads; ++i) {
        ReadData r;
        r.id = "read_" + std::to_string(i);
        r.seq.assign(read_length, 'A');
        r.qual.assign(read_length, 'I');
        reads.push_back(std::move(r));
    }

    namespace fs = std::filesystem;
    fs::path path = fs::temp_directory_path() / "benchmark_writer_medium.fastq.gz";

    for (auto _ : state) {
        fq::io::FastqWriter writer(path.string());
        for (const auto& r : reads) {
            fq::io::FastqRecord rec{};
            rec.id = r.id;
            rec.comment = {};
            rec.seq = r.seq;
            rec.qual = r.qual;
            rec.plus = "+";
            writer.write(rec);
        }
        ::benchmark::DoNotOptimize(path.string());
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.counters["reads"] = static_cast<double>(num_reads);

    fs::remove(path);
}

static void BM_FastQWriter_Parameterized(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    const std::size_t read_length = 150;

    struct ReadData {
        std::string id;
        std::string seq;
        std::string qual;
    };

    std::vector<ReadData> reads;
    reads.reserve(num_reads);
    for (std::size_t i = 0; i < num_reads; ++i) {
        ReadData r;
        r.id = "read_" + std::to_string(i);
        r.seq.assign(read_length, 'A');
        r.qual.assign(read_length, 'I');
        reads.push_back(std::move(r));
    }

    namespace fs = std::filesystem;
    fs::path path = fs::temp_directory_path() / "benchmark_writer_param.fastq.gz";

    for (auto _ : state) {
        fq::io::FastqWriter writer(path.string());
        for (const auto& r : reads) {
            fq::io::FastqRecord rec{};
            rec.id = r.id;
            rec.comment = {};
            rec.seq = r.seq;
            rec.qual = r.qual;
            rec.plus = "+";
            writer.write(rec);
        }
        ::benchmark::DoNotOptimize(path.string());
    }

    state.SetItemsProcessed(state.iterations() * static_cast<long long>(num_reads));
    state.counters["reads"] = static_cast<double>(num_reads);

    fs::remove(path);
}

BENCHMARK(BM_FastQReader_Small)->Unit(::benchmark::kMillisecond);
BENCHMARK(BM_FastQReader_Medium)->Unit(::benchmark::kMillisecond);
BENCHMARK(BM_FastQWriter_Small)->Unit(::benchmark::kMillisecond);
BENCHMARK(BM_FastQWriter_Medium)->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_FastQReader_Parameterized)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_FastQWriter_Parameterized)
    ->Args({10000})
    ->Args({50000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

}  // namespace fq::benchmark
