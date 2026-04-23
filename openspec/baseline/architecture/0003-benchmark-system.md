# RFC-0003: Benchmark System

> **Status**: Active  
> **Created**: 2026-04-17  
> **Last Updated**: 2026-04-23  
> **Authors**: FastQTools Core Team  
> **Migrated From**: legacy benchmark system design document in the pre-OpenSpec docs tree

## Context

FastQTools requires a systematic approach to performance benchmarking, data collection, visualization, and regression detection to ensure performance improvements are maintained over time.

## Decision

### Architecture

The benchmark system follows a modular design with four core components:

```
┌─────────────────────────────────────────────────────────────┐
│                     Benchmark System                         │
├─────────────────────────────────────────────────────────────┤
│  ┌───────────┐  ┌──────────┐  ┌──────────────────────────┐  │
│  │   CLI     │  │ CI Script│  │  GitHub Actions          │  │
│  │ Interface │  │ (bash)   │  │  Workflow                │  │
│  └─────┬─────┘  └────┬─────┘  └──────────┬───────────────┘  │
│        │              │                   │                   │
│        └──────────────┼───────────────────┘                   │
│                       ▼                                       │
│  ┌───────────────────────────────────────────────────────┐   │
│  │                  Benchmark Runner                      │   │
│  │  ┌──────────┐ ┌──────────┐ ┌────────────────────┐     │   │
│  │  │IO Bench  │ │FilterBench│ │Stat Bench          │     │   │
│  │  │(read/write)│(pipeline) │ │(analysis)          │     │   │
│  │  └──────────┘ └──────────┘ └────────────────────┘     │   │
│  └───────────────────────┬───────────────────────────────┘   │
│                          ▼                                    │
│  ┌───────────────────────────────────────────────────────┐   │
│  │                  Data Collector                        │   │
│  │  • JSON output format                                  │   │
│  │  • System metadata collection                          │   │
│  │  • Git commit tracking                                 │   │
│  └───────────────────────┬───────────────────────────────┘   │
│                          ▼                                    │
│  ┌─────────────────────────────────────────────────────────┐│
│  │                    Storage Layer                         ││
│  │  docs/benchmark-reports/                                 ││
│  │  ├── results/           # Historical results             ││
│  │  ├── baselines/         # Named baselines                ││
│  │  ├── charts/            # Generated charts               ││
│  │  ├── latest.md          # Latest summary                 ││
│  │  └── latest.json        # Latest data                    ││
│  └─────────────────────────────────────────────────────────┘│
│                          ▼                                   │
│  ┌───────────────────────────────────────────────────────┐   │
│  │           Report Generator (Python)                    │   │
│  │  ┌──────────┐ ┌──────────┐ ┌────────────────────┐     │   │
│  │  │Chart Gen │ │Markdown  │ │Regression          │     │   │
│  │  │(matplotlib)│Generator │ │Detector            │     │   │
│  │  └──────────┘ └──────────┘ └────────────────────┘     │   │
│  └───────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### Components

#### 1. Benchmark Runner (C++)

Based on Google Benchmark framework:

```cpp
namespace fq::benchmark {

struct BenchmarkConfig {
    std::vector<size_t> threadCounts = {1, 2, 4, 8};
    std::vector<size_t> readCounts = {10000, 100000, 1000000};
    std::vector<size_t> readLengths = {100, 150, 250};
    std::string inputFile;
    std::string outputDir = "docs/benchmark-reports";
    bool ciMode = false;
};

struct BenchmarkResult {
    std::string name;
    std::string category;  // "io", "filter", "stat"
    size_t iterations;
    double meanTimeNs;
    double stdDevNs;
    double minTimeNs;
    double maxTimeNs;
    double throughputMBps;
    double throughputReadsPerSec;
    size_t peakMemoryBytes;
    size_t threadCount;
    size_t inputSize;
};

struct SystemMetadata {
    std::string cpuModel;
    int coreCount;
    size_t memoryBytes;
    std::string osVersion;
    std::string compilerVersion;
    std::string gitCommit;
    std::string gitBranch;
    std::string timestamp;
};

struct BenchmarkReport {
    SystemMetadata metadata;
    std::vector<BenchmarkResult> results;
};

}  // namespace fq::benchmark
```

#### 2. Data Collector

Collects system metadata and formats output:
- System metadata collection (CPU, memory, OS)
- Git information (commit, branch)
- JSON serialization/deserialization

#### 3. Report Generator (Python)

Uses matplotlib for visualization:
- Throughput trend charts
- Configuration comparison charts
- Markdown report generation
- README-embeddable performance summaries

#### 4. Regression Detector

Automated regression detection:
- Warning threshold: 10% degradation
- Critical threshold: 20% degradation
- CI mode exit codes (0=success, 1=critical regression)

### Data Model

#### JSON Result Format

```json
{
  "metadata": {
    "timestamp": "2026-01-09T10:30:00Z",
    "git_commit": "abc123def",
    "git_branch": "main",
    "cpu_model": "AMD Ryzen 9 5900X",
    "core_count": 12,
    "memory_bytes": 34359738368,
    "os_version": "Debian GNU/Linux 12",
    "compiler_version": "clang 21.0.0"
  },
  "results": [
    {
      "name": "BM_FastQReader_SmallFile",
      "category": "io",
      "iterations": 100,
      "mean_time_ns": 1234567890,
      "std_dev_ns": 12345678,
      "min_time_ns": 1200000000,
      "max_time_ns": 1300000000,
      "throughput_mbps": 150.5,
      "throughput_reads_per_sec": 100000,
      "peak_memory_bytes": 104857600,
      "thread_count": 4,
      "input_size": 100000
    }
  ]
}
```

### CLI Interface

```bash
# Main entry script: scripts/tools/benchmark

Usage: benchmark <command> [options]

Commands:
  run         Execute benchmarks
  report      Generate performance reports
  compare     Compare two benchmark results
  baseline    Manage performance baselines

Options:
  --ci        CI mode, GitHub Actions-friendly output
  --threads   Specify thread counts (default: 1,2,4,8)
  --input     Specify input file
  --output    Specify output directory
```

### Correctness Properties

#### Property 1: JSON Output Completeness

*For any* benchmark execution that completes successfully, the output JSON SHALL contain all required fields: `timestamp`, `git_commit`, `git_branch` in metadata, and `name`, `iterations`, `mean_time_ns`, `std_dev_ns`, `throughput_mbps`, `peak_memory_bytes` for each result entry.

#### Property 2: Metric Collection Completeness

*For any* benchmark category (io, filter, stat), collected results SHALL include throughput metrics (MB/s or reads/sec) and timing metrics (mean, std_dev, min, max).

#### Property 3: Configuration Propagation

*For any* benchmark configuration specifying thread counts and input sizes, results SHALL reflect those exact configurations with one result entry per configuration combination.

#### Property 4: Historical Data Preservation

*For any* sequence of N benchmark runs, `docs/benchmark-reports/results/` SHALL contain exactly N distinct result files with unique timestamp-based filenames.

#### Property 5: Regression Detection Thresholds

*For any* metric comparison where `current_value > baseline_value * (1 + threshold)`, the RegressionDetector SHALL flag with appropriate severity: "warning" for threshold=0.10, "critical" for threshold=0.20.

#### Property 6: CI Exit Code Consistency

*For any* CI mode run, exit code SHALL be 0 if no critical regressions detected, non-zero if any critical regression detected.

### Error Handling

| Error Type | Handling |
|-----------|----------|
| Input file not found | Log error, skip benchmark, continue |
| Invalid FASTQ format | Report validation error, exit with error |
| Out of memory | Log error, record partial results, continue |
| Benchmark timeout | Record timeout, move to next |
| System metadata failure | Use "unknown" for missing fields |

### CI Exit Codes

| Exit Code | Meaning |
|-----------|---------|
| 0 | Success, no regressions |
| 1 | Critical regression detected |
| 2 | Benchmark execution failed |
| 3 | Invalid configuration |
| 4 | Input validation failed |

### Testing Strategy

1. **Unit Tests**: JSON serialization, regression threshold calculations, Markdown generation
2. **Property-Based Tests**: JSON structure validation, severity classification, data generator correctness
3. **Integration Tests**: End-to-end execution, report generation, CI mode verification, baseline workflow

## Consequences

### Positive
- Systematic performance tracking over time
- Automated regression detection in CI
- Visual reports for performance analysis
- Baseline management for release comparisons

### Negative
- Adds Google Benchmark dependency
- Requires maintaining test data files
- Python dependency for report generation

## Related Documents

- [Product Specification](../product/fastq-processing.md)
- [Core Architecture](0001-core-architecture.md)
- [Benchmark Guide](../../../docs/dev/benchmark-guide.md)
