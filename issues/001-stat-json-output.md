---
id: 001
title: stat 机器可读 JSON 输出
status: closed
labels: [enhancement]
created: 2026-07-28
closed: 2026-08-18
---

## 现象

`stat` 当前只输出 TSV：摘要行（`#ReadNum`/`#MaxReadLength`/`#Q20` 等）加逐位置明细表，以及可选的签名 sidecar（也是 TSV）。没有机器可读的 JSON 输出。

影响：下游流水线接入 QC 指标时必须自行解析 TSV；README 对比表中也需如实标注与 fastp（HTML + JSON 报告）的差距。这是已知缺口，不是路线图承诺。

## 根因

产品面刻意收窄时只落地了 TSV；指标计算已经集中在 `buildStatisticsReport`，缺的是同一指标集的第二种序列化，而不是新统计。

## 处理

增加 `stat --json <path>`，由 `formatStatisticsJson` 手写序列化同一指标集（不把 nlohmann_json 拉进 CLI）。`-` 可写 stdout，但不能与 `-o -` 同时使用。

## 复盘

机器可读输出应复用已有汇总结构，而不是再算一遍或引入新依赖。CLI 体积和指标一致性比“用现成 JSON 库”更重要。
