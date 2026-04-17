# Benchmark 使用指南

本文档说明如何使用 FastQTools 的性能基准测试系统。

## 快速开始

```bash
# 构建 benchmark 目标
cmake --build build/clang-release --target benchmarks

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

### `benchmark compiler`

运行 GCC vs Clang 编译器对比基准。

```bash
./scripts/benchmark compiler [options]

Options:
  --data-dir         指定测试数据目录
  --output, -o       指定输出 JSON 文件路径
  --benchmark-filter 过滤要运行的测试（正则表达式）
  --repetitions      重复次数
  --compiler         指定编译器（可重复传入）
  --skip-build       跳过构建步骤
  --clean            清理构建目录后重建
  --no-time          禁用最大内存统计

Examples:
  ./scripts/benchmark compiler
  ./scripts/benchmark compiler --benchmark-filter "Reader"
  ./scripts/benchmark compiler --output docs/performance/benchmark-reports/compiler/custom.json
```

### `benchmark visualize`

生成编译器对比可视化报告（Markdown/HTML）。

```bash
./scripts/benchmark visualize [options]

Options:
  --input, -i     输入 JSON 文件
  --output, -o    输出报告路径
  --format, -f    输出格式 (markdown, html, summary)
  --no-charts     禁用图表生成

Examples:
  ./scripts/benchmark visualize
  ./scripts/benchmark visualize --format html
  ./scripts/benchmark visualize --input docs/performance/benchmark-reports/compiler/latest.json
```

## 目录结构

```
docs/performance/benchmark-reports/
├── baselines/         # 命名基线
│   └── <name>.json
├── data/              # 测试数据
│   ├── small_10k.fastq
│   ├── medium_100k.fastq
│   └── large_1m.fastq
├── results/           # 历史测试结果
│   └── YYYY-MM-DD_HH-MM-SS_<commit>.json
├── charts/            # 生成图表
│   └── *.svg
├── compiler/          # 编译器对比报告
│   └── latest.md
├── latest.md
└── latest.json
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

## 测试方法论

### 测试环境

基准测试在以下标准化环境中执行：

- **操作系统**: Ubuntu 22.04 LTS
- **编译器**: 现代 Clang / GCC 15
- **构建类型**: Release (-O3)
- **CPU**: 多核 x86_64 处理器
- **内存**: 16GB+ RAM

### 测试数据

使用合成 FASTQ 数据进行测试，确保结果可重复：

| 数据集 | Reads 数量 | Read 长度 | 文件大小 |
|--------|-----------|-----------|----------|
| Small | 10,000 | 150bp | ~2.5 MB |
| Medium | 100,000 | 150bp | ~25 MB |
| Large | 1,000,000 | 150bp | ~250 MB |

## 基准测试类别

### IO 基准测试

测试 FASTQ 文件的读取和写入性能：

- `BM_FastQReader_*`: 读取性能测试
- `BM_FastQWriter_*`: 写入性能测试（含 gzip 压缩）

### Filter 基准测试

测试不同过滤条件下的处理性能：

- `BM_Filter_NoFilter`: 无过滤（基线）
- `BM_Filter_MinLength`: 最小长度过滤
- `BM_Filter_MinQuality`: 最小质量过滤
- `BM_Filter_MaxNRatio`: N 比例过滤
- `BM_Filter_Combined`: 组合过滤

### Stat 基准测试

测试统计分析功能的性能：

- `BM_Stat_Basic`: 基本统计
- `BM_Stat_BaseComposition`: 碱基组成统计
- `BM_Stat_QualityDistribution`: 质量分布统计
- `BM_Stat_LengthDistribution`: 长度分布统计
- `BM_Stat_Full`: 完整统计

## 性能优化建议

1. **使用多线程**: FastQTools 支持并行处理，使用 `--threads` 参数
2. **批量处理**: 处理大文件时使用批量模式
3. **压缩输出**: 使用 `.gz` 扩展名自动启用压缩
