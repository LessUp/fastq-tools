---
layout: home

hero:
  name: FastQTools
  text: 高性能 FASTQ 处理工具
  tagline: 在标准硬件上处理 170 万 reads/秒，零拷贝设计，规范驱动，易于审计和集成。
  actions:
    - theme: brand
      text: 快速开始
      link: /guide/getting-started
    - theme: alt
      text: GitHub
      link: https://github.com/LessUp/fastq-tools
    - theme: alt
      text: 性能数据
      link: /performance/benchmark-report
    - theme: alt
      text: API 文档
      link: /api/overview

features:
  - icon: ⚡
    title: 极致性能
    details: 170 万 reads/秒处理能力（单核）。Intel oneTBB 无锁并行流水线。最小化分配与缺页。
  - icon: 📖
    title: 零拷贝设计
    details: 所有记录处理基于 std::string_view。在大型 FASTQ 文件或流式缓冲上最小化开销。
  - icon: 📋
    title: 规范驱动
    details: 每个 API 决策和文件格式都在 openspec/baseline 中文档化。易于审计、预测和集成。
---

## 适合哪些任务

| 任务 | 说明 |
|------|------|
| **快速检查 FASTQ 质量** | `FastQTools stat` 一遍完成：读段数、最大读长、总碱基数、碱基组成、GC 含量、质量统计，以及可选的轻量 sidecar。 |
| **过滤与修剪原始数据** | `FastQTools filter` 组合长度、质量、N 比例条件，并在一次处理中完成低质量端修剪。 |
| **嵌入 C++ 工具链** | 复用公开 API 的零拷贝 FASTQ 原语，无需重新实现基础处理逻辑。 |

## 架构概览

![FastQTools Architecture](assets/images/architecture.svg)

流水线使用 Intel oneTBB 实现无锁并行，全程采用零拷贝 `std::string_view`。记录从 I/O 流经处理阶段，无额外分配开销。

## 性能对比

| 特性 | FastQTools | seqtk | fastp |
|------|------------|-------|-------|
| **吞吐量** | 1.7M reads/s | ~500K reads/s | ~800K reads/s |
| **内存模型** | 零拷贝，有界池 | 基于缓冲 | 自适应 |
| **并行性** | TBB 流水线（自动扩展） | 单线程 | 多线程 |
| **QC sidecar** | ✅ 内置 | ❌ | ✅ HTML 报告 |
| **质量修剪** | ✅ AVX2 优化 | ✅ 基础 | ✅ 内置 |
| **接头修剪** | ✅ | ❌ | ✅ 自动检测 |
| **签名统计** | ✅ k-mer 签名 | ❌ | ❌ |
| **C++ API** | ✅ 公共头文件 | ❌ | ❌ |
| **规范驱动** | ✅ 完整基线 | ❌ | ❌ |

::: info 基准条件
AMD Ryzen 9 5900X，10 万 reads × 150bp，单线程读写，并行处理。
:::

## 快速统计

| 操作 | 速度 | 核心 |
|------|------|------|
| FASTQ 读取 | 1696 MB/s | 1× |
| FASTQ 写入 | 1.76M reads/s | 1× |
| 完整统计扫描 | 302 MB/s | 并行 |
| 组合过滤 | 1.67M reads/s | 并行 |

查看 [完整基准测试](performance/benchmark-report) 了解方法论和硬件详情。

## 选择你的路径

| 我想… | 从这里开始 |
|-------|-----------|
| 构建并运行第一个命令 | [快速开始](guide/getting-started) |
| 查看命令语法和示例 | [CLI 参考](guide/cli-reference) |
| 了解基准测试方法论 | [性能概览](performance/benchmark-report) |
| 在 C++ 中集成库 | [API 概览](api/overview) |
| 参与项目开发 | [开发者指南](dev/) |

## 为什么规范很重要

FastQTools 从设计之初就是**规范驱动**的。每个公共 API、文件格式决策和性能保证都在版本控制的规范中记录。这意味着：

- **易于审计**：没有隐藏行为。阅读 [`openspec/baseline/api/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline/api) 获取精确的 API 合约。
- **易于扩展**：提案放在 [`openspec/changes/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/changes)。审查者可以在代码编写前看到影响。
- **易于维护**：归档决策在 [`openspec/archive/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/archive)。无需重新发现旧选择的"为什么"。
