# Benchmark — 性能基准测试

基于 [Google Benchmark](https://github.com/google/benchmark) 的性能基准测试套件。

## C++ 基准测试

| 文件 | 测试内容 |
|------|----------|
| `fastq_io_benchmark.cpp` | FASTQ 读写吞吐量（Reader/Writer） |
| `filter_benchmark.cpp` | Filter 命令端到端性能 |
| `stat_benchmark.cpp` | Stat 命令统计计算性能 |
| `pipeline_benchmark.cpp` | Sequential / oneTBB / Taskflow 公平对照 |
| `object_pool_benchmark.cpp` | ObjectPool 对象池分配性能 |

### 构建与运行

```bash
# 构建所有基准测试
cmake --build build --target benchmarks

# 运行单个基准测试
./build/tools/benchmark/benchmark_fastq_io --benchmark_format=json

# 运行所有基准测试（1M×150 bp、5 次重复、JSON + median/CV 摘要）
cmake --build build/clang-release --target run_benchmarks

# 完整基准测试（5 次重复，输出 raw JSON、reads/s、MiB/s、峰值内存和 CV）
cmake --build build/clang-release --target benchmark_full

# Taskflow 可选 backend 构建完成后，生成 p50/p95/RSS 对照报告
./scripts/core/build --taskflow --build-dir build/taskflow-release
cmake -S . -B build/taskflow-release -DENABLE_TASKFLOW_BACKEND=ON -DBUILD_BENCHMARKS=ON
cmake --build build/taskflow-release --target benchmark_backend_comparison
# 报告：build/taskflow-release/benchmark-results/backends/summary.md
```

## 分析脚本（`scripts/` 子目录）

| 脚本 | 用途 |
|------|------|
| `report_generator.py` | 从 JSON 结果生成 Markdown 报告和 SVG 图表 |
| `regression_detector.py` | 对比两次测试结果，检测性能回归（10% 警告 / 20% 严重） |
| `badge_generator.py` | 生成 shields.io 性能徽章 |
| `gen_benchmark_data.py` | 生成多规模、多参数的 FASTQ 测试数据集 |
| `run_baseline.sh` | 端到端基线测试脚本（多线程 × 多过滤条件） |

### 依赖

- **Python 3.8+**
- **matplotlib**（可选，用于图表生成）

## CMake 目标

| 目标 | 说明 |
|------|------|
| `benchmarks` | 构建所有基准测试可执行文件 |
| `run_benchmarks` | 运行基准测试并输出 JSON |
| `benchmark_full` | 完整测试（3 次重复，聚合结果） |
| `benchmark_ci` | CI 模式（控制台 + JSON 双输出） |
| `benchmark_backend_comparison` | 7 次重复的 backend p50/p95、吞吐与峰值 RSS 对照 |
