#include <benchmark/benchmark.h>
#include <fqtools/io/fastq_reader.h>
#include <fqtools/io/fastq_writer.h>

#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <vector>

namespace fq::benchmark {

// 生成测试数据的辅助函数
class TestDataGenerator {
public:
    static std::string generateFastQRecord(std::size_t read_length = 150) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> base_dis(0, 3);
        static std::uniform_int_distribution<> qual_dis(33, 73); // Phred+33

        static const char bases[] = "ATGC";

        std::ostringstream oss;
        oss << "@read_" << gen() << "\n";

        // 生成序列
        for (std::size_t i = 0; i < read_length; ++i) {
            oss << bases[base_dis(gen)];
        }
        oss << "\n+\n";

        // 生成质量分数
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

// 在临时目录下创建 FASTQ 文件（覆盖同名文件）
std::filesystem::path write_temp_fastq(const std::string& prefix, const std::string& content) {
    namespace fs = std::filesystem;
    fs::path path = fs::temp_directory_path() / (prefix + ".fastq");

    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    ofs << content;
    ofs.close();

    return path;
}

} // namespace

// FastQ 读取基准测试（使用新的 fq::io::FastqReader）
static void BM_FastQReader_SmallFile(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    const std::size_t read_length = 150;

    // 预先生成测试数据并写入临时文件（不计入基准时间）
    std::string test_data = TestDataGenerator::generateFastQFile(num_reads, read_length);
    std::filesystem::path path = write_temp_fastq("benchmark_fastq_reader_smallfile", test_data);
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

    state.SetItemsProcessed(state.iterations() * num_reads);
    state.SetBytesProcessed(state.iterations() * static_cast<long long>(file_size));

    std::filesystem::remove(path);
}

// FastQ 写入基准测试（使用新的 fq::io::FastqWriter）
static void BM_FastQWriter_SmallFile(::benchmark::State& state) {
    const std::size_t num_reads = static_cast<std::size_t>(state.range(0));
    const std::size_t read_length = 150;

    struct ReadData {
        std::string id;
        std::string seq;
        std::string qual;
    };

    // 准备测试数据（仅一次，不计入基准时间）
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
    fs::path path = fs::temp_directory_path() / "benchmark_fastq_writer.fastq.gz";

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

    state.SetItemsProcessed(state.iterations() * num_reads);

    fs::remove(path);
}

// 注册基准测试
BENCHMARK(BM_FastQReader_SmallFile)
    ->Range(1000, 100000)
    ->Unit(::benchmark::kMillisecond);

BENCHMARK(BM_FastQWriter_SmallFile)
    ->Range(1000, 100000)
    ->Unit(::benchmark::kMillisecond);

} // namespace fq::benchmark
