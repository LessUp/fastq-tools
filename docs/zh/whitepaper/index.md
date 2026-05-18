---
title: 技术白皮书
description: 以受控的产品边界、算法层与证据层理解 FastQTools，而不是把它当成“命令合集”。
layout: doc
---

<SectionLandingGrid locale="zh" section="whitepaper" />

# 技术白皮书

## 系统全景

只有把 FastQTools 当作“面向日常 FASTQ 质控的工程内核”，它的价值才会真正清晰。当前维护中的产品面不承诺全能测序平台，而是聚焦 `stat`、`filter`、可嵌入的 C++ 接口，以及支撑这些能力的执行与内存政策。

## 评审问题

白皮书层的目标，是让采用者在不先读源码的前提下回答三类问题：

1. **今天真正维护中的边界是什么？** FastQTools 承诺的是统计、过滤、修剪、gzip 感知 I/O 与 FASTQ QC 所需的执行约束，而不是泛化成任意下游流程平台。
2. **什么系统模型解释了性能姿态？** 吞吐主张必须回到零拷贝记录视图、有界 `FastqBatch` 生命周期与 oneTBB 阶段编排，而不是一句“因为 C++ 所以快”。
3. **证据链放在哪里？** 架构 RFC、benchmark 政策、研究引用与实现参考页都需要保持可达，方便后续复核。

## 这一层到底要证明什么

这一章不是把 README 拉长，而是建立一种评审姿态：把 FastQTools 看成一个边界清晰、非目标明确、并且为后续架构、算法、性能与参考章节预留验证路径的受控系统。

本页背后的核心边界文件包括 <ReferenceBadge kind="Spec" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/product/fastq-processing.md">产品规范</ReferenceBadge>、<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge> 与 <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md">RFC-0006</ReferenceBadge>。

## 边界契约

FastQTools 最适合的场景，是在更重的下游流程开始前，为 FASTQ 数据提供稳定、可复核的 QC 与预处理核心。因此：

- **可以讨论**：summary statistics、逐位点指标、支持的过滤谓词、支持的修剪 mutator、gzip 输入输出，以及保证吞吐和内存有界的执行控制；
- **不应夸大**：把 FastQTools 写成工作流平台、分类器、比对器，或者暗示它维护着超出 `stat` / `filter` 的产品面；
- **可以做对照**：和其他 QC 工具比较定位、接口边界与证据风格，但不能把尚未落地的能力写成当前特性。

因此，白皮书页更像一份技术尽调材料，而不是“功能宣传页”。

## 阅读建议

这套白皮书内容最好按技术评审的顺序逐层往下读。

## 阅读路线

建议按技术评审的顺序往下读：

1. [`架构`](../architecture/) —— 先看边界账本、控制界面与故障收敛策略；
2. [`算法`](../algorithms/) —— 再检查过滤、统计、执行策略和内存约束如何落到今天维护中的行为；
3. [`性能总览`](../performance/) —— 把 benchmark 包络、方法学、风险边界与复现实验路径一起解读；
4. [`研究附录`](../research/) —— 在主结论清楚之后，再阅读书目、同类项目语境与设计演进说明。

如果你还在判断是否值得采用，建议先看 [`为什么选择 FastQTools`](../why-fastqtools/)；当你已经需要具体命令、API 或实现材料时，再转到 [`参考导航`](../reference/)。

## 如何按层继续下钻

上面的阅读路线刻意保持“叙事先行、系统随后、证据再后、操作最后”的顺序，目的是让读者始终沿着同一套系统心智模型前进。

## 站内核心参考

<ReferenceList locale="zh" variant="site" />
