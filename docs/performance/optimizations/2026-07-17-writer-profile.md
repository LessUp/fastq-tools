# Writer 热点复核：未触发优化门槛

## 结论

编号 12 不修改 Writer 实现。v4 基线确实覆盖了 plain、gzip-1/6/9 以及 single/batch API，但当前 WSL2 环境的 `real_time` 被放大到约 `1e7 ms`，快照没有可用于函数级归因的 inclusive CPU profile。因此不能证明 gzip 占 Writer CPU ≥60%，也不能证明序列化占比 ≥20%。按路线图，优化门槛未满足，保留现有实现。

## 已有能力

- `FastqWriterOptions::compressionLevel` 已支持 gzip 1–9，默认仍为 level 6。
- Writer 已使用可配置输出缓冲区，并通过 batch API 进行批量拼接。
- v4 baseline 已保存每个压缩级别的原始 JSON、median/CV 和峰值 RSS，后续可在原生 Linux 复核。

## 复核记录

| 项目 | 结果 |
|------|------|
| v4 生产基准 | 5 次重复完成；I/O CV 均低于 1%，但绝对 `real_time` 受 WSL2 时钟限制 |
| `perf record` | 当前 WSL2 内核缺少匹配的 `linux-tools`，无法生成采样报告 |
| inclusive CPU ≥60% / 序列化 ≥20% | 无可信函数级数据，判定为未满足 |
| 代码变更 | 无 |

## 下一步

在原生 Linux 或提供匹配内核工具的环境重新采集 Writer profile。只有达到阈值且优化后 median 提升至少 10%、结果逐字段一致、峰值内存增长不超过 5% 时，才建立优化前后快照并修改实现。
