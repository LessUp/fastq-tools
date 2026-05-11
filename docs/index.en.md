---
hide:
  - navigation
  - toc
---

<div class="hero-section" markdown>

<p class="hero-eyebrow">面向生物信息学质控的高性能 FASTQ 工具</p>

# FastQTools

<p class="lead">在标准硬件上处理 170 万 reads/秒，零拷贝设计，规范驱动，易于审计和集成。</p>

<div class="hero-actions" markdown>
[快速开始](guide/getting-started.en.md){ .md-button .md-button--primary }
[GitHub](https://github.com/LessUp/fastq-tools){ .md-button }
[性能数据](performance/benchmark-report.md){ .md-button }
[API 文档](api/overview.en.md){ .md-button }
</div>

<p class="hero-note">为研究者和流程工程师设计：轻量、可预测、完全基于规范的 FASTQ 工具。</p>

</div>

## 三大核心优势

<div class="grid cards" markdown>

-   ⚡ **极致性能**

    ---

    170 万 reads/秒处理能力（单核）。Intel oneTBB 无锁并行流水线。最小化分配与缺页。

-   📖 **零拷贝设计**

    ---

    所有记录处理基于 `std::string_view`。在大型 FASTQ 文件或流式缓冲上最小化开销。

-   📋 **规范驱动**

    ---

    每个 API 决策和文件格式都在 [`openspec/baseline/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline) 中文档化。易于审计、预测和集成。

</div>

## 适合哪些任务

<div class="grid cards" markdown>

-   :material-file-search-outline: **快速检查 FASTQ 质量**

    ---

    `FastQTools stat` 一遍完成：读段数、最大读长、总碱基数、碱基组成、GC 含量、质量统计，以及可选的轻量 sidecar。

-   :material-content-cut: **过滤与修剪原始数据**

    ---

    `FastQTools filter` 组合长度、质量、N 比例条件，并在一次处理中完成低质量端修剪。

-   :material-lan-connect: **嵌入 C++ 工具链**

    ---

    复用公开 API 的零拷贝 FASTQ 原语，无需重新实现基础处理逻辑。

</div>

## 架构概览

![FastQTools Architecture](assets/images/architecture.svg){ .center }

流水线使用 Intel oneTBB 实现无锁并行，全程采用零拷贝 `std::string_view`。记录从 I/O 流经处理阶段，无额外分配开销。

## 性能对比

| 特性 | FastQTools | seqtk | fastp |
|------|------------|-------|-------|
| **吞吐量** | 170 万 reads/s | ~50 万 reads/s | ~80 万 reads/s |
| **内存模型** | 零拷贝、有界池 | 缓冲区 | 自适应 |
| **并行度** | TBB 流水线（自动扩展） | 单线程 | 多线程 |
| **QC sidecar** | ✅ 内置 | ❌ | ✅ HTML 报告 |
| **质量修剪** | ✅ AVX2 优化 | ✅ 基础 | ✅ 内置 |
| **接头修剪** | ✅ | ❌ | ✅ 自动检测 |
| **签名统计** | ✅ k-mer 签名 | ❌ | ❌ |
| **C++ API** | ✅ 公开头文件 | ❌ | ❌ |
| **规范驱动** | ✅ 完整基线 | ❌ | ❌ |

!!! note "基准测试条件"
    AMD Ryzen 9 5900X，100K reads × 150bp，单线程读写，并行处理。

## 性能指标

| 操作 | 速度 | 并行度 |
|-----|------|--------|
| FASTQ 读 | 1696 MB/s | 单核 |
| FASTQ 写 | 176 万 reads/s | 单核 |
| 完整统计 | 302 MB/s | 并行 |
| 过滤与修剪 | 167 万 reads/s | 并行 |

详见 [完整性能总览](performance/benchmark-report.md)。

## 文档导航

| 我想… | 从这里开始 |
| --- | --- |
| 构建并运行第一条命令 | [快速开始](guide/getting-started.en.md) |
| 查看命令语法和示例 | [CLI 参考](guide/cli-reference.en.md) |
| 理解性能基准方法 | [性能总览](performance/benchmark-report.md) |
| 从 C++ 集成库 | [API 概览](api/overview.en.md) |
| 参与项目开发 | [开发者指南](dev/index.en.md) |

## 为什么选择规范驱动开发

<p markdown>

FastQTools 从设计之初就采用**规范驱动**方法。每个公开 API、文件格式决策、性能承诺都在版本控制的规范中明确记录。这意味着：

- **易于审计**：没有隐藏行为。请查看 [`openspec/baseline/api/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline/api) 了解完整的 API 契约。
- **易于扩展**：新提案在 [`openspec/changes/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/changes) 中讨论。审阅者可以在代码编写前看到全面影响。
- **易于维护**：已归档决策在 [`openspec/archive/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/archive) 中。不会再为"为什么这样设计"而困惑。

</p>

---

<p style="text-align: center; color: var(--md-default-fg-color--light);">
  <a href="index.md">中文版</a> ·
  <a href="https://github.com/LessUp/fastq-tools">GitHub</a> ·
  <a href="https://github.com/LessUp/fastq-tools/blob/master/LICENSE">MIT 许可证</a>
</p>
