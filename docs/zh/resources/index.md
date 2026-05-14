# 资源中心

资源中心收拢那些**不适合放进主叙事、但对采用与维护都很重要**的材料：仓库入口、贡献方式、规范事实来源、历史归档，以及用于建立背景知识的外部参考。

## 站内入口：从项目事实到协作方式

### 项目与协作

- [`参与贡献`](../contributing) —— 适合准备提交文档、测试或代码改动的人；
- [`变更记录（双语，英文优先）`](../release-notes/changelog) —— 查看最近落地内容；
- [GitHub 仓库](https://github.com/LessUp/fastq-tools) —— 源码、Issue、Discussion、Release 总入口；
- [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) —— 跟踪具体问题与任务。

### 规范与架构事实来源

这些链接不一定是第一次阅读就需要，但当你要验证“项目长期承诺了什么”时非常关键。为避免与旧写法混淆，这里统一按 `RFC-000X` 引用 OpenSpec 架构文档：

- [OpenSpec baseline 总览](https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline)
- [RFC-0001: Core Architecture](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
- [RFC-0002: Toolchain Policy](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)
- [RFC-0003: Benchmark System](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md)
- [RFC-0006: Benchmark Maintenance Policy](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md)

### 自动化与 Agent 文档

- [`领域模型`](../agents/domain)
- [`Issue Tracker`](../agents/issue-tracker)
- [`分类标签`](../agents/triage-labels)

这些页面更偏向维护与自动化协作，不是新用户必须阅读的部分，但对长期接手仓库的人很有价值。

### 历史与迁移材料

- [`归档概览`](../archive/)
- [`迁移通知`](../archive/migration-notice-2026-04-17)

当你看到旧文档、旧脚本或历史讨论时，这组资料能帮助你判断哪些内容仍然有效，哪些已经被 OpenSpec 接管。

## 站外背景：理解 FASTQ 与 QC 语境

### FASTQ 基础参考

- [FASTQ format（Wikipedia）](https://en.wikipedia.org/wiki/FASTQ_format) —— 快速复习格式结构与质量分值历史；
- [NCBI SRA submit formats](https://www.ncbi.nlm.nih.gov/sra/docs/submitformats/) —— 了解 FASTQ 作为交换格式时的实际背景。

### 质量控制与修剪语境

- [FastQC](https://www.bioinformatics.babraham.ac.uk/projects/fastqc/) —— 查看行业内常见 QC 报告语境；
- [Cutadapt documentation](https://cutadapt.readthedocs.io/en/stable/) —— adapter trimming 术语与实践；
- [fastp](https://github.com/OpenGene/fastp) —— 对照一体化 FASTQ 预处理工具。

### 相关工具与对照阅读

- [seqtk](https://github.com/lh3/seqtk) —— 轻量 FASTA/FASTQ 处理工具；
- [zlib-ng](https://github.com/zlib-ng/zlib-ng) / [libdeflate](https://github.com/ebiggers/libdeflate) —— 理解项目压缩路径背后的依赖选择；
- [oneTBB](https://github.com/oneapi-src/oneTBB) —— 理解并行流水线所依赖的并发库。

## 推荐的使用方式

| 你现在的需要 | 建议先点哪里 |
| --- | --- |
| 我想参与项目 | [`参与贡献`](../contributing) |
| 我想确认官方事实来源 | OpenSpec baseline 与相关 ADR |
| 我想理解站外背景知识 | FASTQ / FastQC / Cutadapt / fastp 链接 |
| 我想查历史与迁移原因 | [`归档概览`](../archive/) |

## 与其他导航页的关系

- 如果你要找**站内阅读路径**，先看 [`知识地图`](../knowledge-map/)
- 如果你要找**具体执行手册**，先看 [`参考导航`](../reference/)
- 如果你要先理解**项目为什么存在**，回到 [`为什么选择 FastQTools`](../why-fastqtools/)
