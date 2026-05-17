---
title: 相关项目
description: 按边界而不是按口号比较 FastQTools 与周边 FASTQ / QC 工具。
layout: doc
---

# 相关项目

本页不是“冠军榜”，而是一张边界对照表，帮助读者在做采用判断前，把 FastQTools 放回更大的 FASTQ/QC 工具语境里理解。

## 对照框架

| 项目 | 典型重心 | 为什么值得在这里比较 |
| --- | --- | --- |
| FastQC | 以报告和质量评估为中心 | 适合对照“报告优先”而不是“可嵌入 QC 内核”的场景。 |
| fastp | 过滤、修剪、报告一体化的预处理工具 | 适合比较“一体化预处理”与“聚焦工具包叙事”的差异。 |
| Cutadapt | 围绕 adapter trimming 建立工作流 | 是讨论 trimming 语义与工作流预期时的重要参照。 |
| seqtk | 小而便携的 FASTA/FASTQ 工具集 | 适合对照“紧凑命令集合”与“分层文档 + API”两种取向。 |
| FastQTools | 聚焦 FASTQ QC 的 CLI + C++ API 内核 | 最适合需要可解释吞吐、内存有界与可嵌入接口的场景。 |

## FastQTools 不打算替代什么

FastQTools 并不声称要覆盖 FastQC、fastp、Cutadapt、seqtk 的全部角色。当前维护中的产品范围更窄：围绕 FASTQ 统计与过滤/修剪，构建一套把执行模型、benchmark 证据与 API 边界写清楚的系统。

因此，比起“谁功能更多”，更有价值的比较维度是：

- 报告导向分析 vs. 可复用 QC 内核；
- “一个命令尽可能做完所有事”的预处理工具 vs. 聚焦维护的能力集合；
- 轻量命令集合 vs. 带有架构叙事和公共 C++ 接口的工程内核。

## 什么时候这类比较最有用

当有人问“为什么不直接用 FastQC、fastp、Cutadapt 或 seqtk？”时，最好的回答通常不是“因为 FastQTools 功能更多”，而是：FastQTools 选择了不同的重心——一个带明确执行与维护政策、可以被审查的 QC 内核。

如果你需要这些比较背后的来源，请继续看 [`书目`](./bibliography)；如果你需要项目内部边界的系统解释，请回到 [`白皮书`](../whitepaper/) 与 [`算法`](../algorithms/)。
