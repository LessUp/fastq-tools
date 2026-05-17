---
title: 技术白皮书
description: 以受控的产品边界、算法层与证据层理解 FastQTools，而不是把它当成“命令合集”。
layout: doc
---

<SectionLandingGrid locale="zh" section="whitepaper" />

# 技术白皮书

## 系统全景

只有把 FastQTools 当作“面向日常 FASTQ 质控的工程内核”，它的价值才会真正清晰：当前维护中的产品面只覆盖 `stat`、`filter`、可嵌入的 C++ 接口，以及支撑这些能力的执行与内存策略，而不是泛化成任何测序任务都要承担的平台。

## 这一层到底要证明什么

白皮书层的目标，是让采用者在不先读源码的前提下回答三类评审问题：

1. **产品边界是否诚实？** 当前 baseline 只承诺聚焦 FASTQ QC：统计、过滤、修剪、gzip 感知 I/O，以及围绕 `stat` / `filter` 的执行控制。
2. **性能叙事是否可解释？** 吞吐主张必须回到零拷贝批处理、`std::string_view` 记录视图、oneTBB 流水线阶段和内存有界策略，而不是抽象成“因为 C++ 所以快”。
3. **这些结论之后能否复核？** 架构 RFC、benchmark 政策和开发者文档都被保留下来，方便把采用叙事和维护中的事实来源一一对应。

本页背后的核心边界文件包括 <ReferenceBadge kind="Spec" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/product/fastq-processing.md">产品规范</ReferenceBadge>、<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge> 与 <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md">RFC-0006</ReferenceBadge>。

## 范围、约束与非目标

FastQTools 最适合的场景，是在更重的下游流程开始前，为 FASTQ 数据提供稳定、可复核的 QC 与预处理核心。因此：

- **可以讨论**：summary statistics、逐位点指标、支持的过滤谓词、支持的修剪 mutator、gzip 输入输出，以及保证吞吐和内存有界的执行控制；
- **不应夸大**：把 FastQTools 写成工作流平台、分类器、比对器，或者暗示它维护着超出 `stat` / `filter` 的产品面；
- **可以做对照**：和其他 QC 工具比较定位、接口边界与证据风格，但不能把尚未落地的能力写成当前特性。

因此，白皮书页更像一份技术尽调材料，而不是“功能宣传页”。

## 阅读建议

这套白皮书内容最好按技术评审的顺序逐层往下读。

## 如何按层继续下钻

建议按技术评审的顺序往下读：

1. [`架构`](../architecture/) —— 先理解分层边界、`source → processing → sink` 执行路径，以及内存复用为什么属于架构问题；
2. [`算法`](../algorithms/) —— 再检查过滤、统计、执行策略和内存约束如何落到今天维护中的行为；
3. [`性能总览`](../performance/) —— 最后把 benchmark 快照和方法学、风险边界一起解读；
4. [`研究附录`](../research/) —— 在主结论清楚之后，再阅读书目、同类项目上下文与设计演进说明。

如果你还在判断是否值得采用，建议先看 [`为什么选择 FastQTools`](../why-fastqtools/)；当你已经需要具体命令、API 或实现材料时，再转到 [`参考导航`](../reference/)。

## 站内核心参考

<ReferenceList locale="zh" variant="site" />
