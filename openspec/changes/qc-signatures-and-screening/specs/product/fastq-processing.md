# Product Spec Delta: FASTQ Processing

## MODIFIED Requirements

### Requirement: 产品可以包含轻量序列智能与 side reports

**原定义**: [`openspec/baseline/product/fastq-processing.md`](../../../../baseline/product/fastq-processing.md)

**变更内容**:

FastQTools 作为 preprocessing engine，SHALL 允许以后续 proposal 的方式增加 lightweight sequence intelligence，例如 k-mer/minimizer signatures、duplicate estimation、contamination hints 与 optional structured side reports。

这些能力 SHALL 继续服务于 preprocessing/QC 场景，而 SHALL NOT 让产品滑向 taxonomy classifier、aligner、assembler 或 workflow platform。

#### Scenario: 在产品范围内加入 lightweight sequence intelligence

- **GIVEN** 项目需要扩展 QC/analysis 能力
- **WHEN** `qc-signatures-and-screening` proposal 生效
- **THEN** 产品 SHALL 允许 lightweight sequence intelligence
- **AND** optional structured side reports SHALL 进入范围
- **AND** classifier / platform 化方向 SHALL 继续属于非目标
