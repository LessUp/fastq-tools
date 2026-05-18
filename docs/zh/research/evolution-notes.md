---
title: 演进说明
description: 追踪架构、内存与 benchmark 政策如何塑造当前 FastQTools 叙事。
layout: doc
---

# 演进说明

FastQTools 今天的文档与架构形态并不是偶然形成的。当前维护中的系统叙事，是若干次把实现顾虑提升为公开技术推理材料的结果。

## RFC 主脊柱

<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge> 建立了分层架构、零拷贝批处理模型与 `source → processing → sink` 执行路径。随后 <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md">RFC-0003</ReferenceBadge>、<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md">RFC-0004</ReferenceBadge> 与 <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md">RFC-0006</ReferenceBadge> 进一步把 benchmark 治理、内存政策与发布纪律提升成正式设计语言。

## 核心架构如何变成稳定脊柱

RFC-0001 的意义不只是代码分层；它让 CLI、API 与 benchmark 叙事可以共享同一套系统框架。

## 内存政策如何进入一等设计语言

RFC-0004 让内存复用与 in-flight 有界策略不再只是实现脚注。只要记录视图依赖批次生命周期，内存政策就属于正确性问题，而不只是优化问题。

## benchmark 发布如何从轶事变成治理对象

RFC-0003 定义了 benchmark 数据如何采集与保存；RFC-0006 则进一步把发布面向的 SLA 语言与 GitHub Pages 上的信息性 benchmark 发布拆开。两者一起把性能从“有趣的数字”变成“维护中的证据”。

## 文档如何镜像工程分工

当前文档把叙事、操作与研究职责分开：

- [`白皮书`](../whitepaper/) 负责产品边界与评审语境；
- [`架构`](../architecture/) 与 [`算法`](../algorithms/) 负责系统与执行推理；
- [`性能总览`](../performance/) 负责证据解读；
- [`参考导航`](../reference/) 负责精确查阅；
- [`研究附录`](../research/) 负责书目、比较语境与历史说明。

## 哪些东西应该继续稳定

即使项目继续增长，关键纪律也应该保持不变：新能力不应再把叙事、实现、证据与参考重新压扁回一个没有层次的首页。当前结构之所以有效，是因为每一层回答的是不同问题，并且对应不同程度的证明要求。
