# Benchmark 使用指南

本文档说明如何使用 FastQTools 的性能基准测试系统。

## 快速开始

```bash
# 构建 benchmark 目标
cmake --build build-clang-release --target benchmarks

# 运行基准测试
./scripts/benchmark run

# 生成报告
./scripts/benchmark report
```

## CLI 命令

### `benchmark run`

运行性能基准测试。

```bash
./scripts/benchmark run [options]

Options:
  --output, -o    指定输出文件路径
  --filter        过滤要运行的测试（正则表达式）
  --repetitions   重复次数（默认: 1）
  --ci            CI 模式

Examples:
  ./scripts/benchmark run
  ./scripts/benchmark run --filter "Reader"
  ./scripts/benchmark run --repetitions 5 --ci
```

### `benchmark report`

从测试结果生成报告。

```bash
./scripts/benchmark report [options]

Options:
  --input, -i     输入 JSON 文件
  --format, -f    输出格式 (markdown, summary, readme)
  --charts        生成图表

Examples:
  ./scripts/benchmark report
  ./scripts/benchmark report --format summary
  ./scripts/benchmark report --charts
```

### `benchmark compare`

比较两个测试结果，检测性能回归。

```bash
./scripts/benchmark compare <baseline> <current> [options]

Options:
  --warning-threshold   警告阈值（默认: 0.10 = 10%）
  --critical-threshold  严重阈值（默认: 0.20 = 20%）
  --ci                  CI 模式输出

Examples:
  ./scripts/benchmark compare baseline.json current.json
  ./scripts/benchmark compare baseline.json current.json --warning-threshold 0.05
```

### `benchmark baseline`

管理性能基线。

```bash
./scripts/benchmark baseline <command> [args]

Commands:
  save <name> [source]  保存基线
  list                  列出所有基线
  delete <name>         删除基线

Examples:
  ./scripts/benchmark baseline save v1.0
  ./scripts/benchmark baseline list
  ./scripts/benchmark baseline delete old-baseline
```

### `benchmark data`

管理测试数据。

```bash
./scripts/benchmark data <command> [args]

Commands:
  generate              生成完整测试数据集
  validate <file>       验证 FASTQ 文件格式

Examples:
  ./scripts/benchmark data generate
  ./scripts/benchmark data validate test.fastq
```

## 目录结构

```
benchmark_results/
├── results/           # 历史测试结果
│   └── YYYY-MM-DD_HH-MM-SS_<commit>.json
├── baselines/         # 命名基线
│   └── <name>.json
├── reports/           # 生成的报告
│   ├── latest.md
│   └── charts/
│       └── *.svg
└── data/              # 测试数据
    ├── small_10k.fastq
    ├── medium_100k.fastq
    └── large_1m.fastq
```

## 添加新的基准测试

### 1. 创建测试文件

在 `tools/benchmark/` 目录下创建新的 `.cpp` 文件：

```cpp
#include <benchmark/benchmark.h>
#include <fqtools/io/fastq_reader.h>

namespace fq::benchmark {

static void BM_MyNewBenchmark(::benchmark::State& state) {
    // 准备测试数据（不计入测试时间）
    // ...

    for (auto _ : state) {
        // 被测试的代码
        // ...
        ::benchmark::DoNotOptimize(result);
    }

    // 设置计数器
    state.SetItemsProcessed(state.iterations() * num_items);
    state.SetBytesProcessed(state.iterations() * bytes);
}

BENCHMARK(BM_MyNewBenchmark)
    ->Args({10000})
    ->Args({100000})
    ->Unit(::benchmark::kMillisecond);

}  // namespace fq::benchmark
```

### 2. 更新 CMakeLists.txt

在 `tools/benchmark/CMakeLists.txt` 中添加新目标：

```cmake
add_benchmark(benchmark_my_new
    my_new_benchmark.cpp
)
```

### 3. 运行测试

```bash
cmake --build build --target benchmark_my_new
./build/tools/benchmark/benchmark_my_new --benchmark_format=json
```

## CI 集成

### GitHub Actions

基准测试在以下情况自动运行：

- Push 到 `main` 或 `develop` 分支
- Pull Request 到 `main` 分支
- 每周一 UTC 00:00 定时运行
- 手动触发

### 回归检测

CI 会自动与基线比较，检测性能回归：

- **警告**: 性能下降 10-20%
- **严重**: 性能下降 >20%（导致 CI 失败）

### 更新基线

在 GitHub Actions 中手动触发 workflow，勾选 "Save results as baseline"。

## 解读结果

### JSON 输出格式

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

### 关键指标

| 指标 | 说明 | 单位 |
|------|------|------|
| `mean_time_ns` | 平均执行时间 | 纳秒 |
| `std_dev_ns` | 标准差 | 纳秒 |
| `throughput_mbps` | 数据吞吐量 | MB/s |
| `throughput_reads_per_sec` | Reads 吞吐量 | reads/s |
| `peak_memory_bytes` | 峰值内存 | bytes |

## 故障排除

### 构建失败

确保已安装 Google Benchmark：

```bash
conan install . --build=missing
```

### 结果不稳定

- 关闭其他占用 CPU 的程序
- 使用 `--repetitions` 增加重复次数
- 在 Release 模式下构建

### 图表生成失败

安装 matplotlib：

```bash
pip install matplotlib
```
