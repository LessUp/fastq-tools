# 统计热点复核：未触发优化门槛

## 结论

编号 13 不修改统计 worker。真实 `FqStatisticWorker` benchmark 的 5 次 CPU 时间从约 0.75 s 到 8.37 s，CV 为 45.64%，超过 CPU benchmark 的 5% 目标；这组数据不能作为稳定的优化前基线。当前环境也没有可用的函数级 inclusive CPU profile，因此不实施 map/hash、归约或 SIMD 改动。

## 证据

- benchmark 直接调用生产 `FqStatisticWorker::calculateStats`，输入为固定 seed=42 的 1M×150 bp 数据集。
- 原始 JSON 和摘要位于 [`v4-baseline/2026-07-17/parts/stat`](../benchmark-reports/v4-baseline/2026-07-17/parts/stat/benchmark_stat.summary.json)。
- 摘要已将 `cv_status` 标记为 `high-cv`，并保留每次 repetition 的 CPU/real time，便于在稳定环境复核。
- WSL2 内核缺少匹配 `linux-tools`，无法通过 `perf record` 得到可归因的 inclusive CPU 百分比。

## 决策

旧的 2026-07-13 Callgrind 数字只作为历史记录，不满足 v4 的 ≥15% inclusive CPU 门槛。保持标量参考实现和现有统计结果契约；不引入架构专用 SIMD，也不承诺性能提升。

## 下一步

在原生 Linux、固定 CPU 频率并确认 CPU clock 稳定后，重新运行 5 次生产 stat benchmark，再采集函数级 profile。只有 profile 达到 ≥15%，且优化后 median 提升至少 10%、结果逐字段一致、峰值内存增长不超过 5% 时，才进入实现阶段。
