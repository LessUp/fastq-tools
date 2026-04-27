# Architecture Delta: 0001 Core Architecture

## MODIFIED Requirements

### Requirement: 执行模型必须区分抽象边界与默认后端

**原定义**: [`openspec/baseline/architecture/0001-core-architecture.md`](../../../../baseline/architecture/0001-core-architecture.md)

**变更内容**:

FastQTools 的批处理执行模型 SHALL 被表述为 execution abstraction，其中至少包括 backend、batch source 与 batch sink 的概念边界。Intel oneTBB SHALL 继续作为默认并行执行后端，直到后续 proposal 明确批准新的默认后端。

该抽象 SHALL 不改变 `FastqBatch` / `FastqRecord` 的 zero-copy 生命周期规则。

#### Scenario: 为后续后端演进建立统一架构边界

- **GIVEN** 当前实现仍以内建 oneTBB pipeline 为主
- **WHEN** 项目需要为 execution backend 演进建立规范边界
- **THEN** 架构 SHALL 以 execution abstraction 描述执行模型
- **AND** oneTBB SHALL 保持默认后端
- **AND** zero-copy 生命周期规则 SHALL 保持不变
