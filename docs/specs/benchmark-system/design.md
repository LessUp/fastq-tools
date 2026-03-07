# Design Document: Benchmark System

## Overview

本设计文档描述 FastQTools Benchmark 系统的技术架构和实现方案。该系统基于 Google Benchmark 框架构建，提供完整的性能测试、数据收集、可视化报告和回归检测能力。

系统采用模块化设计，分为以下核心组件：
- **Benchmark Runner**: 基于 Google Benchmark 的测试执行引擎
- **Data Collector**: 性能数据收集和存储模块
- **Report Generator**: 可视化报告生成器（Python 实现）
- **Regression Detector**: 性能回归检测器
- **CLI Interface**: 统一的命令行接口

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     Benchmark System                             │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │
│  │   CLI       │  │  CI Script  │  │  GitHub Actions         │  │
│  │  Interface  │  │  (bash)     │  │  Workflow               │  │
│  └──────┬──────┘  └──────┬──────┘  └───────────┬─────────────┘  │
│         │                │                     │                 │
│         └────────────────┼─────────────────────┘                 │
│                          ▼                                       │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │                  Benchmark Runner                          │  │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────────────┐  │  │
│  │  │ IO Bench    │ │ Filter Bench│ │ Stat Bench          │  │  │
│  │  │ (read/write)│ │ (pipeline)  │ │ (analysis)          │  │  │
│  │  └─────────────┘ └─────────────┘ └─────────────────────┘  │  │
│  └───────────────────────────┬───────────────────────────────┘  │
│                              ▼                                   │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │                  Data Collector                            │  │
│  │  • JSON output format                                      │  │
│  │  • System metadata collection                              │  │
│  │  • Git commit tracking                                     │  │
│  └───────────────────────────┬───────────────────────────────┘  │
│                              ▼                                   │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                    Storage Layer                             ││
│  │  benchmark_results/                                          ││
│  │  ├── results/           # Historical results                 ││
│  │  │   └── YYYY-MM-DD_HH-MM-SS_<commit>.json                  ││
│  │  ├── baselines/         # Named baselines                    ││
│  │  │   └── <name>.json                                         ││
│  │  └── reports/           # Generated reports                  ││
│  │      ├── latest.md                                           ││
│  │      └── charts/                                             ││
│  └─────────────────────────────────────────────────────────────┘│
│                              ▼                                   │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │              Report Generator (Python)                     │  │
│  │  ┌─────────────┐ ┌─────────────┐ ┌─────────────────────┐  │  │
│  │  │ Chart Gen   │ │ Markdown    │ │ Regression          │  │  │
│  │  │ (matplotlib)│ │ Generator   │ │ Detector            │  │  │
│  │  └─────────────┘ └─────────────┘ └─────────────────────┘  │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

## Components and Interfaces

### 1. Benchmark Runner (C++)

基于 Google Benchmark 框架，扩展现有的 `tools/benchmark/` 实现。

```cpp
namespace fq::benchmark {

// 基准测试配置
struct BenchmarkConfig {
    std::vector<size_t> threadCounts = {1, 2, 4, 8};
    std::vector<size_t> readCounts = {10000, 100000, 1000000};  // small, medium, large
    std::vector<size_t> readLengths = {100, 150, 250};
    std::string inputFile;           // 用户提供的输入文件（可选）
    std::string outputDir = "benchmark_results";
    bool ciMode = false;
};

// 基准测试结果
struct BenchmarkResult {
    std::string name;
    std::string category;            // "io", "filter", "stat"
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

// 系统元数据
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

// 完整的基准测试报告
struct BenchmarkReport {
    SystemMetadata metadata;
    std::vector<BenchmarkResult> results;
};

// 基准测试执行器接口
class IBenchmarkRunner {
public:
    virtual ~IBenchmarkRunner() = default;
    virtual BenchmarkReport run(const BenchmarkConfig& config) = 0;
    virtual void saveResults(const BenchmarkReport& report, const std::string& path) = 0;
};

}  // namespace fq::benchmark
```

### 2. Data Collector

负责收集系统信息和格式化输出。

```cpp
namespace fq::benchmark {

class DataCollector {
public:
    // 收集系统元数据
    static SystemMetadata collectSystemMetadata();
    
    // 获取 Git 信息
    static std::string getGitCommit();
    static std::string getGitBranch();
    
    // 序列化为 JSON
    static std::string toJson(const BenchmarkReport& report);
    static BenchmarkReport fromJson(const std::string& json);
};

}  // namespace fq::benchmark
```

### 3. Report Generator (Python)

使用 Python 实现可视化报告生成，利用 matplotlib 生成图表。

```python
# tools/benchmark/report_generator.py

from dataclasses import dataclass
from typing import List, Dict, Optional
import json
import matplotlib.pyplot as plt

@dataclass
class BenchmarkResult:
    name: str
    category: str
    iterations: int
    mean_time_ns: float
    std_dev_ns: float
    throughput_mbps: float
    throughput_reads_per_sec: float
    peak_memory_bytes: int
    thread_count: int
    input_size: int

@dataclass
class BenchmarkReport:
    metadata: Dict
    results: List[BenchmarkResult]

class ReportGenerator:
    """生成性能报告和可视化图表"""
    
    def load_results(self, path: str) -> BenchmarkReport:
        """加载 JSON 格式的基准测试结果"""
        pass
    
    def generate_throughput_chart(self, reports: List[BenchmarkReport], 
                                   output_path: str) -> None:
        """生成吞吐量趋势图"""
        pass
    
    def generate_comparison_chart(self, report: BenchmarkReport,
                                   output_path: str) -> None:
        """生成配置对比图（线程数、文件大小）"""
        pass
    
    def generate_markdown_report(self, report: BenchmarkReport,
                                  output_path: str) -> None:
        """生成 Markdown 格式报告"""
        pass
    
    def generate_readme_section(self, report: BenchmarkReport) -> str:
        """生成可嵌入 README 的性能摘要"""
        pass
```

### 4. Regression Detector

```python
# tools/benchmark/regression_detector.py

@dataclass
class RegressionResult:
    metric_name: str
    baseline_value: float
    current_value: float
    change_percent: float
    severity: str  # "ok", "warning", "critical"

class RegressionDetector:
    """检测性能回归"""
    
    def __init__(self, warning_threshold: float = 0.10,
                 critical_threshold: float = 0.20):
        self.warning_threshold = warning_threshold
        self.critical_threshold = critical_threshold
    
    def compare(self, baseline: BenchmarkReport, 
                current: BenchmarkReport) -> List[RegressionResult]:
        """比较两个报告，返回回归检测结果"""
        pass
    
    def has_regression(self, results: List[RegressionResult]) -> bool:
        """检查是否存在回归"""
        pass
    
    def format_report(self, results: List[RegressionResult]) -> str:
        """格式化回归报告"""
        pass
```

### 5. CLI Interface

统一的命令行接口脚本。

```bash
# scripts/tools/benchmark
# 主入口脚本

Usage: benchmark <command> [options]

Commands:
  run         执行基准测试
  report      生成性能报告
  compare     比较两个基准测试结果
  baseline    管理性能基线
  
Options:
  --ci        CI 模式，输出适合 GitHub Actions 的格式
  --threads   指定线程数（默认: 1,2,4,8）
  --input     指定输入文件
  --output    指定输出目录
```

## Data Models

### JSON 结果格式

```json
{
  "metadata": {
    "timestamp": "2026-01-09T10:30:00Z",
    "git_commit": "abc123def",
    "git_branch": "main",
    "cpu_model": "Intel Core i7-12700K",
    "core_count": 12,
    "memory_bytes": 34359738368,
    "os_version": "Ubuntu 22.04",
    "compiler_version": "clang 19.0.0"
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

### 目录结构

```
benchmark_results/
├── results/                          # 历史结果
│   ├── 2026-01-09_10-30-00_abc123.json
│   └── 2026-01-08_15-20-00_def456.json
├── baselines/                        # 命名基线
│   ├── release-1.0.json
│   └── pre-optimization.json
├── reports/                          # 生成的报告
│   ├── latest.md
│   ├── performance-summary.md        # README 嵌入用
│   └── charts/
│       ├── throughput-trend.svg
│       ├── thread-scaling.svg
│       └── memory-usage.svg
└── data/                             # 测试数据
    ├── small_10k.fastq
    ├── medium_100k.fastq
    └── large_1m.fastq
```

## Correctness Properties

*A property is a characteristic or behavior that should hold true across all valid executions of a system-essentially, a formal statement about what the system should do. Properties serve as the bridge between human-readable specifications and machine-verifiable correctness guarantees.*

### Property 1: JSON Output Completeness

*For any* benchmark execution that completes successfully, the output JSON SHALL contain all required fields: timestamp, git_commit, git_branch in metadata, and name, iterations, mean_time_ns, std_dev_ns, throughput_mbps, peak_memory_bytes for each result entry.

**Validates: Requirements 2.1, 2.2, 2.3**

### Property 2: Metric Collection Completeness

*For any* benchmark category (io, filter, stat), the collected results SHALL include throughput metrics (MB/s or reads/sec) and timing metrics (mean, std_dev, min, max).

**Validates: Requirements 1.2, 1.3, 1.4, 1.5**

### Property 3: Configuration Propagation

*For any* benchmark configuration specifying thread counts and input sizes, the results SHALL reflect those exact configurations in the output, with one result entry per configuration combination.

**Validates: Requirements 1.6, 1.7**

### Property 4: Historical Data Preservation

*For any* sequence of N benchmark runs, the benchmark_results/results/ directory SHALL contain exactly N distinct result files, each with a unique timestamp-based filename.

**Validates: Requirements 2.4, 2.5**

### Property 5: Regression Detection Thresholds

*For any* metric comparison where current_value > baseline_value * (1 + threshold), the Regression_Detector SHALL flag the metric with the appropriate severity: "warning" for threshold=0.10, "critical" for threshold=0.20.

**Validates: Requirements 4.1, 4.2, 4.4, 4.5**

### Property 6: CI Exit Code Consistency

*For any* benchmark run in CI mode, the exit code SHALL be 0 if no critical regressions are detected, and non-zero if any critical regression is detected.

**Validates: Requirements 5.2, 5.4**

### Property 7: Data Generator Correctness

*For any* data generation request with parameters (read_count, min_length, max_length), the generated FASTQ file SHALL contain exactly read_count records, each with sequence length in [min_length, max_length].

**Validates: Requirements 7.2, 7.3**

### Property 8: Input Validation

*For any* input file provided to the benchmark system, IF the file is not a valid FASTQ format, THEN the system SHALL report a validation error before attempting to benchmark.

**Validates: Requirements 7.4**

### Property 9: File Format Support

*For any* valid FASTQ file (compressed or uncompressed), the benchmark system SHALL successfully read and process the file, producing valid benchmark results.

**Validates: Requirements 7.5**

### Property 10: Markdown Report Validity

*For any* generated Markdown report, the output SHALL be syntactically valid Markdown containing at least one table with performance metrics.

**Validates: Requirements 3.3, 3.6**

### Property 11: Documentation Update Preservation

*For any* documentation update operation, existing content outside the designated performance section markers SHALL remain unchanged after the update.

**Validates: Requirements 8.3, 8.4**

### Property 12: Baseline Management

*For any* set of named baselines saved to the system, the list command SHALL return all baseline names, and each baseline SHALL be loadable for comparison.

**Validates: Requirements 6.2, 6.4, 6.5**

### Property 13: Error Resilience

*For any* benchmark suite containing N benchmarks where M benchmarks fail (M < N), the system SHALL complete execution of the remaining (N - M) benchmarks and produce results for them.

**Validates: Requirements 1.8**

## Error Handling

### Benchmark Execution Errors

| Error Type | Handling Strategy |
|------------|-------------------|
| Input file not found | Log error, skip benchmark, continue with others |
| Invalid FASTQ format | Report validation error, exit with error code |
| Out of memory | Log error, record partial results, continue |
| Benchmark timeout | Record timeout, move to next benchmark |
| System metadata collection failure | Use "unknown" for missing fields, continue |

### Report Generation Errors

| Error Type | Handling Strategy |
|------------|-------------------|
| Missing result files | Report warning, generate partial report |
| Chart generation failure | Log error, generate text-only report |
| Invalid baseline | Report error, suggest available baselines |
| Write permission denied | Report error with suggested fix |

### CI Mode Error Codes

| Exit Code | Meaning |
|-----------|---------|
| 0 | Success, no regressions |
| 1 | Critical regression detected |
| 2 | Benchmark execution failed |
| 3 | Invalid configuration |
| 4 | Input validation failed |

## Testing Strategy

### Unit Tests

Unit tests focus on individual component correctness:

- **DataCollector**: Test JSON serialization/deserialization, system metadata collection
- **RegressionDetector**: Test threshold calculations, severity classification
- **ReportGenerator**: Test Markdown generation, table formatting
- **Input validation**: Test FASTQ format validation logic

### Property-Based Tests

Property-based tests validate universal properties across many inputs:

- **Property 1 (JSON Completeness)**: Generate random benchmark results, verify JSON structure
- **Property 5 (Regression Thresholds)**: Generate random metric pairs, verify correct severity
- **Property 7 (Data Generator)**: Generate with random parameters, verify output constraints
- **Property 11 (Documentation Preservation)**: Generate random existing content, verify preservation

### Integration Tests

- End-to-end benchmark execution with small test data
- Report generation from stored results
- CI mode execution and exit code verification
- Baseline save/load/compare workflow

### Test Configuration

- Property-based tests: minimum 100 iterations per property
- Test framework: Google Test for C++, pytest with hypothesis for Python
- Each property test tagged with: **Feature: benchmark-system, Property N: {property_text}**

