# Architecture Delta: 0001 Core Architecture

## MODIFIED Requirements

### Requirement: 后续演进顺序与默认执行后端

**原定义**: [`openspec/baseline/architecture/0001-core-architecture.md`](../../../baseline/architecture/0001-core-architecture.md)

**变更内容**:

`next-gen-fastq-engine` 只建立后续演进入口，不直接替换当前核心架构。未来架构级演进 SHALL 继续拆分为独立 proposal，并按以下顺序推进：

1. `execution-abstraction`
2. `memory-resource-policy`
3. `advanced-preprocessing`
4. `qc-signatures-and-screening`

在后续 proposal 明确批准新的执行后端之前，Intel oneTBB SHALL 继续作为默认并行执行后端。

#### Scenario: 规划后续架构演进

- **GIVEN** FastQTools 需要从当前架构继续向 preprocessing engine 演进
- **WHEN** 团队规划 `next-gen-fastq-engine` 之后的架构工作
- **THEN** 架构演进 SHALL 作为独立 proposal 分阶段落地
- **AND** proposal 顺序 SHALL 依次为 `execution-abstraction`、`memory-resource-policy`、`advanced-preprocessing`、`qc-signatures-and-screening`
- **AND** oneTBB SHALL 保持默认执行后端，直到后续 proposal 明确修改该基线
