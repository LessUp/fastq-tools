# 知识地图

FastQTools 文档站现在分成三层：**叙事层讲判断与导航，执行层讲具体操作，资源层讲补充背景与协作入口**。这张知识地图的目的，是帮助你按问题域而不是按目录名理解整个站点。

## 先理解这张地图的读法

- **叙事层**：回答“为什么值得看、先看哪里、数字该怎么读”；
- **执行层**：回答“命令怎么写、API 怎么用、开发怎么验证”；
- **资源层**：提供补充背景、协作入口、历史归档与外部参照。

如果你把整个站点想成一篇技术白皮书：

1. 首页负责建立兴趣与证据入口；
2. 叙事页负责建立判断框架；
3. 参考页负责让你真正落地；
4. 资源页负责把站内材料和站外知识连起来。

<DiagramFrame
  asset="reading-map"
  caption="阅读地图：先回答叙事问题，再进入执行细节，最后回到资源层补充背景。"
/>

## 按问题域阅读

### 1. 评估：这个项目是否适合我的 FASTQ QC 场景？

从这些页面开始：

- [`为什么选择 FastQTools`](../why-fastqtools/) —— 先看定位、差异化与适用边界；
- [`架构`](../architecture/) —— 再看性能主张背后的结构原因；
- [`性能总览`](../performance/) —— 最后判断 benchmark 证据是否与你的场景同类。

补充背景（外部）：

- [FASTQ format（Wikipedia）](https://en.wikipedia.org/wiki/FASTQ_format) —— 适合快速回忆格式结构；
- [NCBI SRA FASTQ guidance](https://www.ncbi.nlm.nih.gov/sra/docs/submitformats/) —— 适合确认 FASTQ 作为交换格式时的基本背景。

### 2. 运行：我现在要把命令跑起来

从这些页面开始：

- [`工作流`](../workflows/) —— 按目标选择阅读路径；
- [`快速开始`](../guide/getting-started) —— 最快跑通第一次执行；
- [`CLI 参考`](../guide/cli-reference) —— 查具体参数与示例；
- [`配置说明`](../guide/configuration) / [`部署指南`](../guide/deployment) —— 处理环境与交付。

补充背景（外部）：

- [FastQC documentation](https://www.bioinformatics.babraham.ac.uk/projects/fastqc/) —— 理解常见 QC 输出语境；
- [Cutadapt documentation](https://cutadapt.readthedocs.io/en/stable/) —— 对照 adapter trimming 常见术语。

### 3. 集成：我要把能力接入自己的系统

先看：

- [`参考导航`](../reference/) —— 按意图选择进入点；
- [`API 概览`](../api/overview) —— 明确公共入口；
- [`IO 模块`](../api/io)、[`处理模块`](../api/processing)、[`统计模块`](../api/statistics) —— 按模块下钻；
- [`架构`](../architecture/) —— 回看边界与数据流。

补充背景（外部）：

- [seqtk](https://github.com/lh3/seqtk) —— 经典轻量工具对照；
- [fastp](https://github.com/OpenGene/fastp) —— 一体化 QC 工具对照。

### 4. 维护：我要确认项目的规范、质量门禁和历史脉络

先看：

- [`开发者指南`](../dev/) —— 本地构建、测试、质量工具入口；
- [`参与贡献`](../contributing) —— 协作方式；
- [`资源中心`](../resources/) —— 汇总仓库入口、Agent 文档、归档索引；
- OpenSpec baseline / ADR：
  - [`Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
  - [`Toolchain Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)
  - [`Benchmark System`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md)

## 站内内容分层速览

| 层级 | 主要页面 | 你会得到什么 |
| --- | --- | --- |
| 叙事层 | [`为什么选择 FastQTools`](../why-fastqtools/)、[`架构`](../architecture/)、[`工作流`](../workflows/)、[`性能总览`](../performance/) | 定位、取舍、阅读路径、证据语境 |
| 执行层 | [`参考导航`](../reference/)、[`guide/`](../guide/)、[`api/`](../api/)、[`dev/`](../dev/) | 具体命令、接口、开发流程 |
| 资源层 | [`资源中心`](../resources/)、[`参与贡献`](../contributing)、[`archive/`](../archive/) | 协作入口、历史材料、延伸链接 |

## 如果你只有 10 分钟

- 想判断项目值不值得继续看：[`为什么选择 FastQTools`](../why-fastqtools/)
- 想快速找到实操入口：[`工作流`](../workflows/)
- 想系统浏览细节资料：[`参考导航`](../reference/)
- 想扩展到站外背景与项目规范：[`资源中心`](../resources/)
