# Product Spec Delta: FASTQ Processing

## MODIFIED Requirements

### Requirement: 运行时演进不得破坏默认用户体验

**原定义**: [`openspec/baseline/product/fastq-processing.md`](../../../../baseline/product/fastq-processing.md)

**变更内容**:

FastQTools 允许以后续 proposal 的方式把底层执行模型从 oneTBB-specific 叙事提升为 execution abstraction，但默认维护中的用户体验 SHALL 继续围绕现有 `stat` 与 `filter` 命令展开。

该演进 SHALL 优先服务于 preprocessing engine 的内部可替换性与性能演进，而不是把产品扩展成 workflow orchestrator。

#### Scenario: 升级执行模型但保持当前命令边界

- **GIVEN** 项目需要为 execution backend 抽象预留入口
- **WHEN** `execution-abstraction` proposal 生效
- **THEN** 产品 SHALL 允许执行模型演进
- **AND** 默认维护命令 SHALL 仍然是 `stat` 与 `filter`
- **AND** workflow orchestration SHALL 继续属于非目标
