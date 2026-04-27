# Architecture Delta: 0001 Core Architecture

## MODIFIED Requirements

### Requirement: 高级 preprocessing 必须作为模块化阶段建模

**原定义**: [`openspec/baseline/architecture/0001-core-architecture.md`](../../../../baseline/architecture/0001-core-architecture.md)

**变更内容**:

FastQTools 的 preprocessing 架构 SHALL 允许将 adapter/primer、pair synchronization、UMI、polyG/polyX 与 low-complexity 能力建模为可组合的模块化阶段。该设计 SHALL 与 execution abstraction 和 memory policy 协同，而 SHALL NOT 绕开既有 zero-copy 生命周期约束。

#### Scenario: 把高级 preprocessing 组织成模块化阶段

- **GIVEN** 后续实现需要增加多个 preprocessing 能力
- **WHEN** 架构定义 advanced preprocessing 边界
- **THEN** 这些能力 SHALL 被组织成可组合模块
- **AND** 它们 SHALL 与 execution abstraction 和 memory policy 协同
- **AND** zero-copy 生命周期约束 SHALL 保持不变
