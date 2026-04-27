# Architecture Delta: 0001 Core Architecture

## MODIFIED Requirements

### Requirement: 内存资源策略必须作为独立策略层建模

**原定义**: [`openspec/baseline/architecture/0001-core-architecture.md`](../../../../baseline/architecture/0001-core-architecture.md)

**变更内容**:

FastQTools 的架构 SHALL 允许将 scratch resource、worker-local pools、allocation telemetry 与可选 allocator hooks 建模为独立的 memory resource policy 层。该策略层 SHALL 与 execution abstraction 协同，但 SHALL NOT 改变 `FastqBatch` / `FastqRecord` 的 zero-copy 生命周期语义。

#### Scenario: 把 memory policy 从隐式优化提升为架构边界

- **GIVEN** 后续实现需要显式管理 scratch memory 与 pool reuse
- **WHEN** memory policy 进入架构设计
- **THEN** 架构 SHALL 将其定义为独立策略层
- **AND** 该策略层 SHALL 与 execution abstraction 协同
- **AND** zero-copy 生命周期语义 SHALL 保持不变
