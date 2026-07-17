# FastQTools v4 基准快照环境

本快照由编号 3 的生产基准套件生成，作为 v4 性能 backlog 的新证据基线。目录中的 JSON 是 Google Benchmark 原始输出和 runner 摘要，不能用旧的 2026-07-13 Valgrind 小数据快照替代。

## 运行口径

| 项目 | 值 |
|------|----|
| 数据集 | 固定 seed=42，1,000,000 reads × 150 bp |
| 输入大小 | 316,888,890 bytes（约 302.2 MiB） |
| 批大小 | 10,000 reads；生产 `FastqReader`/`FastqWriter` |
| Writer 场景 | plain、gzip-1、gzip-6、gzip-9；single 与 batch API |
| filter 场景 | 生产 Pipeline、MinLength/MinQuality/MaxNRatio Predicate、QualityTrimmer |
| stat 场景 | 生产 `FqStatisticWorker` |
| 重复次数 | 5；每个重复执行一次完整 workload（`--benchmark_min_time=0.001`） |
| 稳定性目标 | CPU benchmark CV ≤5%；文件 I/O CV ≤10% |
| 随机性 | `std::mt19937(42)`，A/T/G/C 和质量值均固定分布 |

## 工具链和机器

| 项目 | 值 |
|------|----|
| CPU | AMD Ryzen 7 5800H，8 核 16 线程 |
| 内存 | 15 GiB |
| OS | Linux 6.18.33.2-microsoft-standard-WSL2 |
| 编译器 | Clang 21.1.8 |
| 构建 | Release，`BUILD_BENCHMARKS=ON` |
| CMake | 3.28.3 |
| Python | 3.12.2 |
| Google Benchmark | Conan 依赖，1.9.5 |

## 可复核命令

```bash
cmake --build build/clang-release --target benchmarks
python3 tools/benchmark/scripts/run_benchmarks.py \
  --build-dir build/clang-release \
  --output-dir docs/performance/benchmark-reports/v4-baseline/2026-07-17 \
  --repetitions 5
```

`--benchmark_min_time=0.001` 由 runner 传给每个 Google Benchmark；它只禁止短 workload 的自动重复，不减少固定 1M 条输入。输出目录同时保存每个 executable 的原始 JSON 和 `.summary.json`。

## 测量限制

当前 sandbox/WSL2 的 monotonic real-time 在部分进程中被放大到约 `1e7 ms`，因此 JSON 的绝对 `real_time` 不可用于跨环境比较。摘要仍保留原始 real/cpu 字段；文件 I/O 的 CV 使用 real-time，stat 的 CPU clock 出现非稳定增长时会在摘要中标记为异常，不据此做热点优化结论。需要绝对延迟或压缩吞吐结论时，应在原生 Linux 复核。
