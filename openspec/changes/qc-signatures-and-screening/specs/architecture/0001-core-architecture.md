# Architecture Delta: 0001 Core Architecture

## MODIFIED Requirements

### Requirement: QC signatures 必须作为可选轻量分析模块建模

**原定义**: [`openspec/baseline/architecture/0001-core-architecture.md`](../../../../baseline/architecture/0001-core-architecture.md)

**变更内容**:

FastQTools 的架构 SHALL 允许将 k-mer/minimizer signatures、duplicate estimation、contamination hints 与 structured side reports 建模为可选轻量分析模块。该模块层 SHALL 建立在 preprocessing engine 之上，而 SHALL NOT 替代现有文本统计路径或扩展为平台级分析系统。

#### Scenario: 把 lightweight signature 能力组织成可选模块

- **GIVEN** 后续实现需要增加 lightweight signature / screening 能力
- **WHEN** 架构定义这些能力的边界
- **THEN** 它们 SHALL 被建模为可选轻量分析模块
- **AND** 现有文本统计路径 SHALL 继续存在
- **AND** 平台级分析系统 SHALL 继续属于非目标
