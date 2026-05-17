---
title: 演进说明
description: 追踪架构、内存与 benchmark 政策如何塑造今天的 FastQTools 叙事。
layout: doc
---

# 演进说明

FastQTools 今天的文档和架构形态并不是偶然形成的。当前维护中的系统叙事，是一系列把“实现问题”提升为“公开可解释材料”的决策叠加结果。

## 核心架构成为稳定主轴

<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge> 确立了分层架构、零拷贝批处理模型，以及 `source → processing → sink` 执行路径。它的意义不只是代码分目录，更在于让 CLI、API 与 benchmark 叙事能共用同一套系统框架。

## 内存策略被提升为一等设计语言

<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md">RFC-0004</ReferenceBadge> 把内存复用与 in-flight 有界性明确写进设计，而不再把分配行为视为“实现顺手优化”。一旦记录视图依赖批次生命周期，内存策略就属于正确性问题，而不只是优化问题。

## Benchmark 发布从“经验数字”变成“受治理的证据”

<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md">RFC-0003</ReferenceBadge> 定义了 benchmark 数据如何采集和保存；<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md">RFC-0006</ReferenceBadge> 则进一步把“发布口径 SLA”与“GitHub Pages 信息性 benchmark”区分开来。两者共同把性能从“有趣的数字”变成了维护中的证据材料。

## 文档结构开始镜像这种工程分层

当前文档把叙事、执行、证据与参考责任拆开：

- [`白皮书`](../whitepaper/) 负责产品范围与评审语境；
- [`架构`](../architecture/) 与 [`算法`](../algorithms/) 负责系统与执行推理；
- [`性能总览`](../performance/) 负责证据解释；
- [`参考导航`](../reference/) 负责精确查阅材料；
- [`研究附录`](../research/) 负责书目、对照语境与历史说明。

这样的拆分有价值，是因为它既让主叙事保持紧凑，又不会牺牲深度评审真正需要的材料。

## 什么原则应该继续稳定保留

即便项目未来继续扩展，也不应该把叙事、实现、证据与参考重新揉回一个没有层次的入口页。当前结构之所以有效，正是因为每一层都在回答不同的问题，并承担不同强度的证明责任。
