---
title: 相关项目
description: 按边界而不是按 hype 比较 FastQTools 与相邻 FASTQ/QC 工具。
layout: doc
---

# 相关项目

这一页不是冠军榜，而是一份边界比较表，用来帮助读者在做采用判断之前，把 FastQTools 放回更大的 FASTQ/QC 工具语境里。

## 比较框架

| 项目 | 典型重心 | 为什么在这里重要 |
| --- | --- | --- |
| FastQC | 以报告为中心的质量评估 | 当问题是“报告分析”而不是“可嵌入 QC 内核”时，这是重要对照。 |
| fastp | 集成式预处理，包含过滤、修剪与报告 | 适合对比“大而全单体命令”与较窄工具包叙事。 |
| Cutadapt | 以 adapter trimming 为中心的工作流 | 是修剪语义与工作流预期的重要参考点。 |
| seqtk | 小型、可移植的 FASTA/FASTQ 工具箱 | 适合对比“紧凑命令集合”与“分层文档 + 公共 API”。 |
| FastQTools | 聚焦 FASTQ QC 的 CLI + C++ API 内核 | 当目标是可解释吞吐、有界内存与可嵌入接口时更合适。 |

## 范围与证据对照

FastQTools 并不声称取代 FastQC、fastp、Cutadapt 或 seqtk 的所有角色。当前维护中的产品范围更窄：围绕例行 FASTQ 统计与过滤/修剪，构建一个执行模型、benchmark 证据与 API 边界被同时记录下来的系统。

因此，比较重点应该放在**边界选择**上：

- 以报告为中心的分析 versus 可复用 QC 内核；
- “一个命令做完所有预处理” versus 更小但长期维护的能力集合；
- 紧凑 utilities versus 明确记录的架构与公共 C++ 表面。

## 它并不想替代什么

当有人问“为什么不用 FastQC、fastp、Cutadapt 或 seqtk？”时，最有力的答案通常不是“FastQTools 功能更多”，而是“FastQTools 选择了不同的重心：一个可审计、执行政策明确、维护边界清晰的 QC 内核”。

## 什么时候这类比较真正有用

需要补充这些比较背后的来源时，请继续看 [`书目`](./bibliography)。需要回到解释这些边界的内部系统叙事时，请回到 [`白皮书`](../whitepaper/) 与 [`算法`](../algorithms/)。
