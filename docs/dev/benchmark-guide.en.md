# Benchmark Usage Guide

This document explains how to use the FastQTools performance benchmark system.

## Quick Start

```bash
# Build benchmark target
cmake --build build/clang-release --target benchmarks

# Run benchmarks
./scripts/tools/performance/benchmark run

# Generate report
./scripts/tools/performance/benchmark report
```

## CLI Commands

### `benchmark run`

Run performance benchmarks.

```bash
./scripts/tools/performance/benchmark run [options]

Options:
  --output, -o    Specify output file path
  --filter        Filter tests to run (regex)
  --repetitions   Repetition count (default: 1)
  --ci            CI mode

Examples:
  ./scripts/tools/performance/benchmark run
  ./scripts/tools/performance/benchmark run --filter "Reader"
  ./scripts/tools/performance/benchmark run --repetitions 5 --ci
```

### `benchmark report`

Generate report from test results.

```bash
./scripts/tools/performance/benchmark report [options]

Options:
  --input, -i     Input JSON file
  --format, -f    Output format (markdown, summary, readme)
  --charts        Generate charts

Examples:
  ./scripts/tools/performance/benchmark report
  ./scripts/tools/performance/benchmark report --format summary
  ./scripts/tools/performance/benchmark report --charts
```

### `benchmark compare`

Compare two test results, detect performance regressions.

```bash
./scripts/tools/performance/benchmark compare <baseline> <current> [options]

Options:
  --warning-threshold   Warning threshold (default: 0.10 = 10%)
  --critical-threshold  Critical threshold (default: 0.20 = 20%)
  --ci                  CI mode output

Examples:
  ./scripts/tools/performance/benchmark compare baseline.json current.json
  ./scripts/tools/performance/benchmark compare baseline.json current.json --warning-threshold 0.05
```

### `benchmark baseline`

Manage performance baselines.

```bash
./scripts/tools/performance/benchmark baseline <command> [args]

Commands:
  save <name> [source]  Save baseline
  list                  List all baselines
  delete <name>         Delete baseline

Examples:
  ./scripts/tools/performance/benchmark baseline save v1.0
  ./scripts/tools/performance/benchmark baseline list
  ./scripts/tools/performance/benchmark baseline delete old-baseline
```

### `benchmark data`

Manage test data.

```bash
./scripts/tools/performance/benchmark data <command> [args]

Commands:
  generate              Generate complete test dataset
  validate <file>       Validate FASTQ file format

Examples:
  ./scripts/tools/performance/benchmark data generate
  ./scripts/tools/performance/benchmark data validate test.fastq
```

### `benchmark compiler`

Run GCC vs Clang compiler comparison benchmark.

```bash
./scripts/tools/performance/benchmark compiler [options]

Options:
  --data-dir         Specify test data directory
  --output, -o       Specify output JSON file path
  --benchmark-filter Filter tests to run (regex)
  --repetitions      Repetition count
  --compiler         Specify compiler (can be passed multiple times)
  --skip-build       Skip build step
  --clean            Clean build directory before rebuilding
  --no-time          Disable max memory statistics

Examples:
  ./scripts/tools/performance/benchmark compiler
  ./scripts/tools/performance/benchmark compiler --benchmark-filter "Reader"
  ./scripts/tools/performance/benchmark compiler --output docs/benchmark-reports/compiler/custom.json
```

### `benchmark visualize`

Generate compiler comparison visualization report (Markdown/HTML).

```bash
./scripts/tools/performance/benchmark visualize [options]

Options:
  --input, -i     Input JSON file
  --output, -o    Output report path
  --format, -f    Output format (markdown, html, summary)
  --no-charts     Disable chart generation

Examples:
  ./scripts/tools/performance/benchmark visualize
  ./scripts/tools/performance/benchmark visualize --format html
  ./scripts/tools/performance/benchmark visualize --input docs/benchmark-reports/compiler/latest.json
```

## Directory Structure

```
docs/benchmark-reports/
├── baselines/         # Named baselines
│   └── <name>.json
├── data/              # Test data
│   ├── small_10k.fastq
│   ├── medium_100k.fastq
│   └── large_1m.fastq
├── results/           # Historical test results
│   └── YYYY-MM-DD_HH-MM-SS_<commit>.json
├── charts/            # Generated charts
│   └── *.svg
├── compiler/          # Compiler comparison reports
│   └── latest.md
├── latest.md
└── latest.json
```

## Adding New Benchmarks

### 1. Create Test File

Create new `.cpp` file in `tools/benchmark/` directory:

```cpp
#include <benchmark/benchmark.h>
#include <fqtools/io/fastq_reader.h>

namespace fq::benchmark {

static void BM_MyNewBenchmark(::benchmark::State& state) {
    // Prepare test data (not counted in test time)
    // ...

    for (auto _ : state) {
        // Code under test
        // ...
        ::benchmark::DoNotOptimize(result);
    }

    // Set counters
    state.SetItemsProcessed(state.iterations() * num_items);
    state.SetBytesProcessed(state.iterations() * bytes);
}

BENCHMARK(BM_MyNewBenchmark)
    ->Args({10000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

}  // namespace fq::benchmark
```

### 2. Update CMakeLists.txt

Add new target in `tools/benchmark/CMakeLists.txt`:

```cmake
add_benchmark(benchmark_my_new
    my_new_benchmark.cpp
)
```

### 3. Run Test

```bash
cmake --build build --target benchmark_my_new
./build/tools/benchmark/benchmark_my_new --benchmark_format=json
```

## CI Integration

### GitHub Actions

Benchmarks automatically run in these scenarios:

- Push to `main` or `develop` branch
- Pull Request to `main` branch
- Weekly Monday UTC 00:00 scheduled run
- Manual trigger

### Regression Detection

CI automatically compares with baseline to detect performance regressions:

- **Warning**: 10-20% performance degradation
- **Critical**: >20% performance degradation (causes CI failure)

### Update Baseline

Manually trigger workflow in GitHub Actions, check "Save results as baseline".

## Interpreting Results

### JSON Output Format

```json
{
  "metadata": {
    "timestamp": "2026-01-12T10:30:00Z",
    "git_commit": "abc123",
    "cpu_model": "Intel Core i7",
    "core_count": 8
  },
  "results": [
    {
      "name": "BM_FastQReader_Medium",
      "category": "io",
      "iterations": 100,
      "mean_time_ns": 50000000,
      "throughput_mbps": 200.5
    }
  ]
}
```

### Key Metrics

| Metric | Description | Unit |
|--------|-------------|------|
| `mean_time_ns` | Mean execution time | nanoseconds |
| `std_dev_ns` | Standard deviation | nanoseconds |
| `throughput_mbps` | Data throughput | MB/s |
| `throughput_reads_per_sec` | Reads throughput | reads/s |
| `peak_memory_bytes` | Peak memory | bytes |

## Troubleshooting

### Build Failure

Ensure Google Benchmark is installed:

```bash
conan install . --build=missing
```

### Unstable Results

- Close other CPU-intensive programs
- Use `--repetitions` to increase repetition count
- Build in Release mode

### Chart Generation Failure

Install matplotlib:

```bash
pip install matplotlib
```

## Testing Methodology

### Test Environment

Benchmarks execute in the following standardized environment:

- **OS**: Ubuntu 22.04 LTS
- **Compiler**: Modern Clang / GCC 15
- **Build Type**: Release (-O3)
- **CPU**: Multi-core x86_64 processor
- **Memory**: 16GB+ RAM

### Test Data

Use synthetic FASTQ data for testing to ensure reproducible results:

| Dataset | Reads Count | Read Length | File Size |
|---------|-------------|-------------|-----------|
| Small | 10,000 | 150bp | ~2.5 MB |
| Medium | 100,000 | 150bp | ~25 MB |
| Large | 1,000,000 | 150bp | ~250 MB |

## Benchmark Categories

### IO Benchmarks

Test FASTQ file read and write performance:

- `BM_FastQReader_*`: Read performance tests
- `BM_FastQWriter_*`: Write performance tests (with gzip compression)

### Filter Benchmarks

Test processing performance under different filtering conditions:

- `BM_Filter_NoFilter`: No filter (baseline)
- `BM_Filter_MinLength`: Min length filter
- `BM_Filter_MinQuality`: Min quality filter
- `BM_Filter_MaxNRatio`: N ratio filter
- `BM_Filter_Combined`: Combined filter

### Stat Benchmarks

Test statistics functionality performance:

- `BM_Stat_Basic`: Basic statistics
- `BM_Stat_BaseComposition`: Base composition statistics
- `BM_Stat_QualityDistribution`: Quality distribution statistics
- `BM_Stat_LengthDistribution`: Length distribution statistics
- `BM_Stat_Full`: Full statistics

## Performance Optimization Recommendations

1. **Use multithreading**: FastQTools supports parallel processing, use `--threads` parameter
2. **Batch processing**: Use batch mode when processing large files
3. **Compressed output**: Use `.gz` extension to automatically enable compression
