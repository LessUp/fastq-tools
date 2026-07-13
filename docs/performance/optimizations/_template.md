<!--
optimization 模板：复制本文件为 YYYY-MM-DD-<slug>.md 后填充。
下划线前缀使其在索引中靠后；不要直接编辑本文件作为条目。
-->

# <一句话标题：优化目标 + 关键手段>

| 字段       | 值 |
|------------|----|
| 日期       | YYYY-MM-DD |
| 目标场景   | <如：FASTQ 读取路径 / stat 完整统计 / backend 对照> |
| 相关模块   | <如：io/reader、processing/stat> |
| 相关 commit | `<sha>` <可选多行> |
| 基线快照   | [链接](../benchmark-reports/<scenario>/<YYYY-MM-DD>/summary.md) |
| 优化后快照 | [链接](../benchmark-reports/<scenario>/<YYYY-MM-DD>/summary.md) |

## 背景

为什么要优化这个场景。业务动机、用户反馈、或基线未达目标的差距。

## 基线数据

引用基线快照，摘录关键指标（不贴全表）。

| 指标 | 基线值 |
|------|--------|
| p50 (ms) | — |
| 吞吐 (MiB/s) | — |
| Peak RSS (MiB) | — |

## 瓶颈分析

profiler 工具与热点定位过程。

- **工具**：<perf、perf record、Intel VTune、Google Benchmark --benchmark_time_unit>
- **热点 1**：<函数/代码位置>，占比 <x>%，原因 <…>
- **热点 2**：…

> 关键火焰图/数据可外链，或归档到 `benchmark-reports/<scenario>/<date>/` 下。

## 优化手段

按改动维度分类，每项链接到 commit。

- **算法/数据结构**：<…>（commit `<sha>`）
- **并行/调度**：<…>（commit `<sha>`）
- **内存/拷贝**：<…>（commit `<sha>`）
- **I/O/批处理**：<…>（commit `<sha>`）

## 优化后数据

同口径对比表。指标列与基线表一致，便于横向比较。

| 指标 | 基线 | 优化后 | 变化 |
|------|------|--------|------|
| p50 (ms) | — | — | -x% |
| 吞吐 (MiB/s) | — | — | +x% |
| Peak RSS (MiB) | — | — | -x MiB |

## 结论与回归点

- 优化是否达到预期；未达到则说明原因与后续计划。
- 副作用：内存、CPU 利用率、其他场景是否退化。
- 回归测试：`./scripts/core/test --filter '<pattern>'` 结果。
- 后续监测点：是否需要加入 CI 性能门槛或定期基准。
