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

    `FastQTools stat` 一遍完成：读段数、长度分布、碱基组成、GC 含量、质量统计。

-   :material-content-cut: **过滤与修剪原始数据**

    ---

    `FastQTools filter` 组合长度、质量、N 比例条件，并在一次处理中完成低质量端修剪。

-   :material-lan-connect: **嵌入 C++ 工具链**

    ---

    复用公开 API 的零拷贝 FASTQ 原语，无需重新实现基础处理逻辑。

</div>

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
  <a href="index.md">English</a> ·
  <a href="https://github.com/LessUp/fastq-tools">GitHub</a> ·
  <a href="https://github.com/LessUp/fastq-tools/blob/master/LICENSE">MIT 许可证</a>
</p>

## 推荐阅读路径

| 你的目标 | 建议入口 |
| --- | --- |
| 先把工具构建起来并跑通第一个命令 | [Getting Started](guide/getting-started.en.md) |
| 想查具体命令和参数 | [CLI Reference](guide/cli-reference.en.md) |
| 想知道性能数字应该怎么理解 | [Benchmark Overview](performance/benchmark-report.md) |
| 想看 C++ 接口组织方式 | [API Overview](api/overview.en.md) |
| 想参与项目改进 | [Contributing](contributing.en.md) |

## 代表性性能数据

| 工作负载 | 代表性结果 |
| --- | --- |
| FASTQ 读取路径 | 1696 MB/s |
| FASTQ 写出路径 | 176 万 reads/s |
| 组合过滤处理 | 167 万 reads/s |
| 完整统计分析 | 302 MB/s |

<p class="muted-note">以上数字来自维护中的 100K reads（150 bp）基准集合，硬件为 AMD Ryzen 9 5900X。它们用于帮助你判断量级，而不是对所有输入和环境作绝对承诺。</p>

## 中文用户的实用入口

<div class="language-note" markdown>

- 仓库说明： [README.zh-CN.md](https://github.com/LessUp/fastq-tools/blob/master/README.zh-CN.md)
- 代码仓库： [LessUp/fastq-tools](https://github.com/LessUp/fastq-tools)
- 英文文档首页： [Documentation Home](index.md)
- 发布记录： [GitHub Releases](https://github.com/LessUp/fastq-tools/releases)

</div>

---

<p style="text-align: center; color: var(--md-default-fg-color--light);">
  <a href="index.md">English home</a> ·
  <a href="https://github.com/LessUp/fastq-tools">GitHub</a> ·
  MIT License
</p>
