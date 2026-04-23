---
hide:
  - navigation
  - toc
---

<div class="hero-section" markdown>

<p class="hero-eyebrow">中文概览</p>

# FastQTools

<p class="lead">面向 FASTQ 质控场景的轻量工具：快速统计、过滤与修剪，以及可嵌入的 C++ API。</p>

<div class="hero-actions" markdown>
[中文 README](https://github.com/LessUp/fastq-tools/blob/master/README.zh-CN.md){ .md-button .md-button--primary }
[英文入门文档](guide/getting-started.en.md){ .md-button }
[CLI 参考](guide/cli-reference.en.md){ .md-button }
[性能总览](performance/benchmark-report.md){ .md-button }
</div>

<p class="hero-note">站点完整导航目前主要维护英文页面；本页提供中文定位与关键入口，帮助你先快速判断 FastQTools 是否适合你的工作流。</p>

</div>

## 适合哪些任务

<div class="grid cards" markdown>

-   :material-file-search-outline: **快速检查 FASTQ 是否健康**

    ---

    用 `stat` 查看读段数量、长度分布、碱基组成、GC 含量和质量指标，适合在下游分析前做首轮 QC。

-   :material-content-cut: **过滤与修剪原始 reads**

    ---

    用 `filter` 组合长度、质量、N 比例条件，并在同一次处理里完成低质量端修剪。

-   :material-lan-connect: **嵌入现有 C++ 工具链**

    ---

    如果你已有自己的分析程序，可以直接复用公开 API，而不是重新实现 FASTQ 基础处理逻辑。

</div>

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
