/**
 * @file pipeline_benchmark.cpp
 * @brief 执行后端公平对照基准
 * @details 三种 backend 共享相同 reader、batch operation、writer 和配置。
 */

#include <benchmark/benchmark.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

#include "processing/execution_backend.h"
#include "processing/execution_runtime.h"

namespace fq::benchmark {

namespace {

struct BenchmarkResult {
    std::uint64_t totalReads = 0;
    std::uint64_t totalBases = 0;
    std::uint64_t checksum = 0;
};

class CpuBenchmarkOperation {
public:
    using result_type = BenchmarkResult;

    [[nodiscard]] auto makeResult() const -> result_type {
        return {};
    }

    auto processBatch(fq::io::FastqBatch& batch) const -> result_type {
        result_type result;
        result.totalReads = batch.size();
        for (const auto& record : batch) {
            result.totalBases += record.seq.size();
            for (const char base : record.seq) {
                result.checksum = result.checksum * 131U + static_cast<unsigned char>(base);
            }
            for (const char quality : record.qual) {
                result.checksum = result.checksum * 131U +
                    static_cast<unsigned char>(quality);
            }
        }
        return result;
    }

    void afterCommit(result_type& /*partial*/, std::uint64_t /*committedBytes*/) const {}

    void merge(result_type& total, result_type partial) const {
        total.totalReads += partial.totalReads;
        total.totalBases += partial.totalBases;
        total.checksum ^= partial.checksum;
    }
};

auto makeInput(size_t readCount, size_t readLength) -> std::string {
    std::string content;
    const auto estimatedRecordBytes = readLength * 2 + 32;
    content.reserve(readCount * estimatedRecordBytes);
    const std::string sequence(readLength, 'A');
    const std::string quality(readLength, 'I');

    for (size_t i = 0; i < readCount; ++i) {
        content += "@read_";
        content += std::to_string(i);
        content += '\n';
        content += sequence;
        content += "\n+\n";
        content += quality;
        content += '\n';
    }
    return content;
}

auto backendName(fq::processing::ExecutionBackendPreference backend) -> std::string_view {
    using fq::processing::ExecutionBackendPreference;
    switch (backend) {
        case ExecutionBackendPreference::Automatic:
            return "automatic";
        case ExecutionBackendPreference::Sequential:
            return "sequential";
        case ExecutionBackendPreference::OneTbb:
            return "onetbb";
        case ExecutionBackendPreference::Taskflow:
            return "taskflow";
    }
    return "unknown";
}

void runBackendBenchmark(::benchmark::State& state,
                         fq::processing::ExecutionBackendPreference backend,
                         bool writeOutput) {
    if (backend == fq::processing::ExecutionBackendPreference::Taskflow &&
        !fq::processing::isTaskflowExecutionBackendAvailable()) {
        state.SkipWithError("Taskflow backend was not enabled at build time");
        return;
    }

    const auto readCount = static_cast<size_t>(state.range(0));
    const auto threadCount = static_cast<size_t>(state.range(1));
    const auto input = std::filesystem::temp_directory_path() /
        ("fqtools_backend_" + std::string(backendName(backend)) + ".fastq");
    const auto output = std::filesystem::temp_directory_path() /
        ("fqtools_backend_" + std::string(backendName(backend)) + "_output.fastq");

    {
        std::ofstream stream(input, std::ios::binary | std::ios::trunc);
        stream << makeInput(readCount, 150);
    }
    const auto inputBytes = std::filesystem::file_size(input);

    for (auto _ : state) {
        fq::processing::ExecutionRuntime runtime;
        fq::processing::ExecutionRuntimeRequest request;
        request.inputPath = input.string();
        if (writeOutput) {
            request.outputPath = output.string();
        }
        request.options.batchSize = 1'000;
        request.options.threadCount = threadCount;
        request.backend = backend;

        CpuBenchmarkOperation operation;
        const auto outcome = runtime.execute(request, operation);
        ::benchmark::DoNotOptimize(outcome.result.totalReads);
        ::benchmark::DoNotOptimize(outcome.result.checksum);
        ::benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(readCount));
    state.SetBytesProcessed(state.iterations() * static_cast<std::int64_t>(inputBytes));
    state.counters["threads"] = static_cast<double>(threadCount);
    state.counters["batch_records"] = 1'000.0;
    state.counters["writes_output"] = writeOutput ? 1.0 : 0.0;

    std::filesystem::remove(input);
    std::filesystem::remove(output);
}

void BM_Backend_SequentialCpu(::benchmark::State& state) {
    runBackendBenchmark(
        state, fq::processing::ExecutionBackendPreference::Sequential, false);
}

void BM_Backend_OneTbbCpu(::benchmark::State& state) {
    runBackendBenchmark(state, fq::processing::ExecutionBackendPreference::OneTbb, false);
}

void BM_Backend_TaskflowCpu(::benchmark::State& state) {
    runBackendBenchmark(state, fq::processing::ExecutionBackendPreference::Taskflow, false);
}

void BM_Backend_SequentialReadWrite(::benchmark::State& state) {
    runBackendBenchmark(
        state, fq::processing::ExecutionBackendPreference::Sequential, true);
}

void BM_Backend_OneTbbReadWrite(::benchmark::State& state) {
    runBackendBenchmark(state, fq::processing::ExecutionBackendPreference::OneTbb, true);
}

void BM_Backend_TaskflowReadWrite(::benchmark::State& state) {
    runBackendBenchmark(state, fq::processing::ExecutionBackendPreference::Taskflow, true);
}

constexpr std::int64_t kBenchmarkReads = 100'000;

BENCHMARK(BM_Backend_SequentialCpu)
    ->Args({kBenchmarkReads, 1})
    ->Unit(::benchmark::kMillisecond)
    ->UseRealTime();
BENCHMARK(BM_Backend_OneTbbCpu)
    ->Args({kBenchmarkReads, 2})
    ->Args({kBenchmarkReads, 4})
    ->Args({kBenchmarkReads, 8})
    ->Unit(::benchmark::kMillisecond)
    ->UseRealTime();
BENCHMARK(BM_Backend_TaskflowCpu)
    ->Args({kBenchmarkReads, 2})
    ->Args({kBenchmarkReads, 4})
    ->Args({kBenchmarkReads, 8})
    ->Unit(::benchmark::kMillisecond)
    ->UseRealTime();

BENCHMARK(BM_Backend_SequentialReadWrite)
    ->Args({kBenchmarkReads, 1})
    ->Unit(::benchmark::kMillisecond)
    ->UseRealTime();
BENCHMARK(BM_Backend_OneTbbReadWrite)
    ->Args({kBenchmarkReads, 2})
    ->Args({kBenchmarkReads, 4})
    ->Args({kBenchmarkReads, 8})
    ->Unit(::benchmark::kMillisecond)
    ->UseRealTime();
BENCHMARK(BM_Backend_TaskflowReadWrite)
    ->Args({kBenchmarkReads, 2})
    ->Args({kBenchmarkReads, 4})
    ->Args({kBenchmarkReads, 8})
    ->Unit(::benchmark::kMillisecond)
    ->UseRealTime();

}  // namespace

}  // namespace fq::benchmark
