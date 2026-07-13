# 测试环境基线

所有 `benchmark-reports/` 快照与 `optimizations/` 对比默认基于本基线。偏离需在快照头注明。

## 硬件

| 项目 | 规格 |
|------|------|
| CPU | AMD Ryzen 9 5900X |
| 核心数 | 12 核 24 线程 |
| 内存 | <待补> |
| 存储 | <待补：NVMe 型号/顺序读写> |
| OS  | Linux 6.18.33.2-microsoft-standard-WSL2 |

> WSL2 环境下 I/O 性能受虚拟化影响，跨环境对比需谨慎。涉及磁盘吞吐的结论建议在原生 Linux 复核。

## 软件工具链

| 项目 | 版本 |
|------|------|
| 编译器 | Clang 21 / GCC 13（CI 默认） |
| 构建类型 | Release |
| CMake | 3.28+ |
| Conan | 2.x |
| oneTBB | <待补：conan 依赖版本> |
| Google Benchmark | 1.14+ |

## 数据集

| 名称 | 规模 | 来源 | 用途 |
|------|------|------|------|
| 默认基准集 | 100K reads，150 bp | 合成生成 | 读写/过滤/统计基准 |
| <待补> | — | — | — |

> 数据集生成脚本/下载链接待补。新数据集需在此登记。

## 调用方式

```bash
# 默认 Release 构建
./scripts/core/build

# 运行基准
cmake --build build --target benchmarks
./build/tools/benchmark/benchmark_fastq_io --benchmark_format=json
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
