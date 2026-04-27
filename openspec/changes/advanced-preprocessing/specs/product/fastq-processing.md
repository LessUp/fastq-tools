# Product Spec Delta: FASTQ Processing

## MODIFIED Requirements

### Requirement: 产品能力可扩展到现代 FASTQ conditioning

**原定义**: [`openspec/baseline/product/fastq-processing.md`](../../../../baseline/product/fastq-processing.md)

**变更内容**:

FastQTools 作为 preprocessing engine，SHALL 允许以后续 proposal 的方式增加 adapter/primer upgrade、paired-end synchronization（含 orphan/singleton policy）、UMI extraction/tag/filter、polyG/polyX handling 与 low-complexity filtering 等现代 FASTQ conditioning 能力。

这些能力 SHALL 继续服务于 preprocessing 场景；其中 UMI SHALL 被视为 preprocessing 能力，而 SHALL NOT 强制引入 dedup workflow，也 SHALL NOT 把产品扩展成 aligner、assembler、taxonomy platform 或 workflow runner。

#### Scenario: 在产品边界内加入现代 preprocessing 能力

- **GIVEN** 项目需要从基础 QC 继续演进
- **WHEN** `advanced-preprocessing` proposal 生效
- **THEN** 产品 SHALL 允许现代 FASTQ conditioning 能力进入范围
- **AND** 这些能力 SHALL 继续服务于 preprocessing 场景
- **AND** UMI SHALL 继续停留在 preprocessing 范围内
- **AND** 平台化方向 SHALL 继续属于非目标
