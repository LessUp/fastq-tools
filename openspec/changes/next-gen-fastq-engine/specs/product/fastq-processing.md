# Product Spec Delta: FASTQ Processing

## MODIFIED Requirements

### Requirement: 产品定位与范围边界

**原定义**: [`openspec/baseline/product/fastq-processing.md`](../../../baseline/product/fastq-processing.md)

**变更内容**:

FastQTools 的产品定位从“maintenance closeout 阶段的 FASTQ QC toolkit”调整为“聚焦的 FASTQ preprocessing engine”。当前维护中的命令仍然只有 `stat` 与 `filter`，并继续以 `include/fqtools/` 暴露的可嵌入 C++ 能力作为公共边界。

项目允许以后续独立 proposal 的方式继续演进 preprocessing 相关能力，但 mapping、assembly、taxonomy platform 与 workflow orchestration 继续明确排除在范围外。

#### Scenario: 保留当前维护命令并重定位产品叙事

- **GIVEN** 当前 baseline 将 FastQTools 描述为 closeout-only QC toolkit
- **WHEN** `next-gen-fastq-engine` 作为后续演进入口生效
- **THEN** 产品 SHALL 被描述为聚焦的 FASTQ preprocessing engine
- **AND** 当前维护中的 CLI 命令 SHALL 仍然是 `stat` 与 `filter`
- **AND** mapping、assembly、taxonomy platform 与 workflow orchestration SHALL 继续被视为非目标
