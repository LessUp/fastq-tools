# 测试环境基线

所有 `benchmark-reports/` 快照与 `optimizations/` 对比默认基于本基线。偏离需在快照头注明。

## 硬件

| 项目 | 规格 |
|------|------|
| CPU | AMD Ryzen 7 5800H with Radeon Graphics |
| 架构 | Zen3，8 核 16 线程，单 socket |
| 基频/加速 | 3.2 GHz / 4.4 GHz |
| L1d / L1i | 32 KiB × 8（每核 8-way，64 B 行） |
| L2 | 512 KiB × 8（每核 8-way） |
| L3 | 16 MiB（共享，16-way） |
| 内存 | 15 GiB DDR4 |
| 存储 | WSL2 虚拟盘（顺序写 ~1.1 GB/s，虚拟化层透传） |
| OS  | Linux 6.18.33.2-microsoft-standard-WSL2 |

> WSL2 环境下 I/O 性能受虚拟化影响，跨环境对比需谨慎。涉及磁盘吞吐的结论建议在原生 Linux 复核。

### PMU 可用性（影响 perf/VTune 选型）

| 资源 | 状态 | 说明 |
|------|------|------|
| 通用 arch 事件 | ✅ 7 个 | cycles / instructions / cache-references / cache-misses / branch-instructions / branch-misses / stalled-cycles-frontend |
| AMD 专有 IBS | ❌ | Hyper-V vPMU 不透传 IBS MSR |
| Uncore（L3/UMC/DF） | ❌ | vPMU 不暴露 uncore 设备 |
| RAPL 功耗 | ❌ | vPMU 不透传 |
| `perf_event_paranoid` | 2 | 默认限制用户态硬件事件访问 |
| `perf` 工具 | ❌ | 无匹配 `linux-tools-6.18.33.2-microsoft` 包 |

**结论**：WSL2 下只能做函数级热点 + 粗粒度 cache 命中率分析。微架构 Top-Down / 内存延迟 / DRAM 带宽 / 功耗分析不可用，需裸金属云实例或原生 Linux。本仓库性能分析采用 Valgrind 指令级模拟 + Google Benchmark 吞吐基准的组合策略。

## 软件工具链

| 项目 | 版本 |
|------|------|
| 编译器 | Clang 21.1.8（默认）/ GCC 15.1.0 |
| 构建类型 | Release（基准）/ RelWithDebInfo（Valgrind 分析，-O2 + -g） |
| CMake | 3.28.3 |
| Conan | 2.19.0 |
| oneTBB | 2022.3.0（Conan 依赖） |
| Google Benchmark | 1.9.5（Conan 依赖） |
| Valgrind | 3.22.0 |
| libc++ | LLVM 21 配套 |

## 数据集

| 名称 | 规模 | 来源 | 用途 |
|------|------|------|------|
| sample_10k_len100 | 10K reads × 100 bp，2.1 MiB | `tests/e2e/.tmp_python/sample_10k_len100.fastq` | Google Benchmark / 端到端 |
| sample_1k | 1K reads × 100 bp，214 KiB | `sample_10k_len100` 前 1000 条 | Valgrind 分析（慢工具用小数据） |
| v4_production_seed42 | 1M reads × 150 bp，316,888,890 bytes | benchmark 进程临时生成，`std::mt19937(42)` | 生产 Reader/Writer/filter/stat 基线 |

> Valgrind 工具慢 20-50x，用 1K reads 子集保证可接受运行时间。v4 生产基准固定使用 1M×150 bp；旧的 10K/100K Google Benchmark 快照只保留为历史记录。

## 调用方式

```bash
# 默认 Release 构建
./scripts/core/build

# Valgrind 分析构建（带调试符号）
./scripts/core/build --type RelWithDebInfo --build-dir build/clang-relwithdebinfo

# 运行 v4 生产基准（raw JSON + median/CV）
cmake --build build/clang-release --target benchmarks
python3 tools/benchmark/scripts/run_benchmarks.py \
  --build-dir build/clang-release \
  --output-dir docs/performance/benchmark-reports/v4-baseline/2026-07-17 \
  --repetitions 5
```

执行 backend 对照（Taskflow 默认不参与生产构建）：

```bash
./scripts/core/build --taskflow --build-dir build/taskflow-release
cmake -S . -B build/taskflow-release \
  -DENABLE_TASKFLOW_BACKEND=ON \
  -DBUILD_BENCHMARKS=ON
cmake --build build/taskflow-release --target benchmark_backend_comparison
```

## 偏离基线的记录要求

快照头需注明任何与上表的差异，至少包含：
- 硬件：CPU/内存/存储变更
- 工具链：编译器版本、构建类型、关键依赖版本
- 数据集：规模、来源、生成方式
- 调用：非默认参数、环境变量、并行度

未注明的字段视为与基线一致。
