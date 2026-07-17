# FastQTools v4 生产基准摘要

## 结论

这是 1M×150 bp 固定数据集、5 次重复的生产路径快照。基准直接调用 Reader/Writer、Processing Pipeline、Predicate/Mutator 和 `FqStatisticWorker`；本阶段没有修改这些热点实现。旧 backlog 中“Writer 无批量缓冲”“不存在 stat benchmark”“reserve/memset 已证明是主瓶颈”等结论均不再作为 v4 优化依据。

## 原始数据

| executable/scenario | 原始 JSON | runner 摘要 |
|---------------------|-----------|-------------|
| Reader + plain Writer | [`parts/io-plain/benchmark_fastq_io.json`](./parts/io-plain/benchmark_fastq_io.json) | [`parts/io-plain/benchmark_fastq_io.summary.json`](./parts/io-plain/benchmark_fastq_io.summary.json) |
| gzip-1 Writer | [`parts/io-gzip1/benchmark_fastq_io.json`](./parts/io-gzip1/benchmark_fastq_io.json) | [`parts/io-gzip1/benchmark_fastq_io.summary.json`](./parts/io-gzip1/benchmark_fastq_io.summary.json) |
| gzip-6 Writer | [`parts/io-gzip6/benchmark_fastq_io.json`](./parts/io-gzip6/benchmark_fastq_io.json) | [`parts/io-gzip6/benchmark_fastq_io.summary.json`](./parts/io-gzip6/benchmark_fastq_io.summary.json) |
| gzip-9 Writer | [`parts/io-gzip9/benchmark_fastq_io.json`](./parts/io-gzip9/benchmark_fastq_io.json) | [`parts/io-gzip9/benchmark_fastq_io.summary.json`](./parts/io-gzip9/benchmark_fastq_io.summary.json) |
| production filter | [`parts/filter/benchmark_filter.json`](./parts/filter/benchmark_filter.json) | [`parts/filter/benchmark_filter.summary.json`](./parts/filter/benchmark_filter.summary.json) |
| production stat | [`parts/stat/benchmark_stat.json`](./parts/stat/benchmark_stat.json) | [`parts/stat/benchmark_stat.summary.json`](./parts/stat/benchmark_stat.summary.json) |
| object pool | [`parts/object-pool/benchmark_object_pool.json`](./parts/object-pool/benchmark_object_pool.json) | [`parts/object-pool/benchmark_object_pool.summary.json`](./parts/object-pool/benchmark_object_pool.summary.json) |
| backend probes | [`parts/object-pool/benchmark_pipeline.json`](./parts/object-pool/benchmark_pipeline.json) | [`parts/object-pool/benchmark_pipeline.summary.json`](./parts/object-pool/benchmark_pipeline.summary.json) |

完整机器、命令、数据集和 WSL2 时钟限制见 [`environment.md`](./environment.md)。

## 中位数摘要

以下数值来自 runner 的 5 次重复（I/O 的绝对 real-time 受 WSL2 时钟影响，适合只在本机做相对比较）：

| 场景 | median reads/s | median MiB/s | CV |
|------|---------------:|-------------:|---:|
| Reader | 202,903 | 61.3 | 0.02% |
| plain single | 117,444 | 35.5 | 0.01% |
| plain batch | 81,759 | 24.7 | 0.01% |
| gzip-1 single | 35,559 | 10.7 | 0.09% |
| gzip-1 batch | 17,623 | 5.3 | 0.07% |
| gzip-6 single | 7,194 | 2.2 | 0.38% |
| gzip-6 batch | 3,632 | 1.1 | 0.33% |
| gzip-9 single | 3,636 | 1.1 | 0.83% |
| gzip-9 batch | 1,686 | 0.5 | 0.90% |
| filter baseline | 125,623 | 38.0 | 0.03% |
| filter combined + trim | 37,948 | 11.5 | 0.01% |
| stat worker | 146,897 | 44.4 | 45.64%（CPU clock 异常） |

各行的完整字段（峰值 RSS、time source、目标 CV 和状态）见对应 `.summary.json`；在 WSL2 时钟限制下，不据绝对时间跨环境做性能优化决策。

## 生产场景观察

| 场景 | 观察 | 决策 |
|------|------|------|
| Reader | 1M 条固定输入，统计 `reads/s`、MiB/s 和峰值 RSS | 仅作为新基线，不凭直觉改 Reader |
| Writer plain | 同时覆盖 single/batch API | 现有 Writer 已有批量缓冲；是否优化由 profile 决定 |
| Writer gzip | 分别覆盖 level 1/6/9 与 single/batch | 只有 gzip inclusive CPU ≥60% 才进入编号 12 |
| filter | 真实 Pipeline + Predicate/Mutator，包含 trim 后过滤场景 | 只在真实 profile 热点 ≥15% 时优化 |
| stat | 真实 `FqStatisticWorker`，输出逐字段统计 | 先解决测量时钟/稳定性，再考虑 SIMD |

## 质量和稳定性

runner 为每个场景计算 median、CV、reads/s、MiB/s、峰值 RSS。文件 I/O 目标 CV≤10%，CPU 目标 CV≤5%；若当前 WSL2 时钟或调度违反目标，记录为测量限制，不把它解释成生产回归。
