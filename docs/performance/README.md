# 性能数据与优化记录

本目录沉淀 FastQTools 的性能测试数据与优化历程。分两层：

| 子目录 | 性质 | 内容 |
|--------|------|------|
| [`benchmark-reports/`](./benchmark-reports/) | 原始快照 | 不可变数据：表格、JSON、profiler 输出。按场景+日期归档。 |
| [`optimizations/`](./optimizations/) | 优化叙事 | 每条记录一次优化：背景、瓶颈分析、手段、before/after 对比、结论。 |

## 阅读顺序

1. [`environment.md`](./environment.md) — 测试环境基线，所有快照的口径。
2. [`optimizations/2026-07-13-backlog.md`](./optimizations/2026-07-13-backlog.md) — 性能优化路线图，待执行的优化方向。
3. [`benchmark-reports/`](./benchmark-reports/) — 历次原始数据。
4. [`optimizations/`](./optimizations/) — 数据背后的故事（已完成的优化叙事）。
5. [`../benchmark.md`](../benchmark.md) — 面向用户的代表性结果摘要。

## 写作规范

- **快照与叙事分离**：`benchmark-reports/` 只放数据，不写分析；分析写进 `optimizations/` 并链接快照。
- **同口径对比**：所有快照必须引用 `environment.md` 中的基线；偏离基线需在快照头注明。
- **文件命名**：`YYYY-MM-DD-<kebab-case-slug>.md`，与 `postmortems/` 一致。
- **快照归档路径**：`benchmark-reports/<scenario>/<YYYY-MM-DD>/`，内含 `summary.md` 与原始输出文件。
- **优化条目必填**：基线快照链接、优化后快照链接、同口径对比表。
- **中文撰写**。

## 索引

### 优化记录

> 按日期倒序。新增后在此追加一行。

| 日期 | 标题 | 场景 | 增益 |
|------|------|------|------|
| —    | （暂无条目） | — | — |

### 快照归档

| 场景 | 路径 | 说明 |
|------|------|------|
| 执行 backend 对照 | [`benchmark-reports/backends/summary.md`](./benchmark-reports/backends/summary.md) | oneTBB vs Taskflow |
| Callgrind 热点分析 | [`benchmark-reports/callgrind/2026-07-13/summary.md`](./benchmark-reports/callgrind/2026-07-13/summary.md) | stat + filter 函数级热点 |
| Cachegrind 缓存分析 | [`benchmark-reports/cachegrind/2026-07-13/summary.md`](./benchmark-reports/cachegrind/2026-07-13/summary.md) | L1/L2/LL 命中率模拟 |
| Memcheck 内存正确性 | [`benchmark-reports/memcheck/2026-07-13/summary.md`](./benchmark-reports/memcheck/2026-07-13/summary.md) | 内存泄露 + 越界检测 |
| Massif 堆内存分析 | [`benchmark-reports/massif/2026-07-13/summary.md`](./benchmark-reports/massif/2026-07-13/summary.md) | 堆内存峰值 + 增长曲线 |
| Helgrind 线程正确性 | [`benchmark-reports/helgrind/2026-07-13/summary.md`](./benchmark-reports/helgrind/2026-07-13/summary.md) | 多线程数据竞争检测 |
| FASTQ IO 吞吐 | [`benchmark-reports/fastq-io/2026-07-13/summary.md`](./benchmark-reports/fastq-io/2026-07-13/summary.md) | Reader/Writer 吞吐基准 |

## 如何跑基准

见 [`../benchmark.md`](../benchmark.md) 的"基准测试工具"章节。简述：

```bash
# 构建基准目标
cmake --build build --target benchmarks

# 运行单个
./build/tools/benchmark/benchmark_fastq_io --benchmark_format=json

# 运行全部
cmake --build build --target run_benchmarks
```

新快照入库前，确认：
1. 测试机与 `environment.md` 一致；不一致则在快照头注明差异。
2. 数据规模、构建类型、编译器版本完整记录。
3. 原始 JSON/CSV 一并归档，便于后续复核。
