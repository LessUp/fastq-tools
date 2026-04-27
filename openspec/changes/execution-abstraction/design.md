# Design: execution-abstraction

> **Proposal**: [proposal.md](./proposal.md)
> **Last Updated**: 2026-04-28
> **Depends On**: [next-gen-fastq-engine](../next-gen-fastq-engine/design.md)

## Context

当前 baseline 把并行执行几乎直接绑定到 `tbb::parallel_pipeline`。这与现有代码状态一致，但不利于后续把 backend policy、pair-aware flow、structured side outputs 和 memory policy 作为独立能力演进。

本设计的目标不是否定 oneTBB，而是先把“执行模型”从“某个具体后端”里分离出来，让 oneTBB 成为默认实现而不是唯一叙事。

## Goals / Non-Goals

### Goals
- 定义 execution backend/source/sink 的规范边界。
- 保持 `FastqBatch` / `FastqRecord` 的 zero-copy 生命周期规则不变。
- 保持 `stat` / `filter` 在默认后端下的行为与输出兼容。

### Non-Goals
- 不承诺立即提供生产级多后端矩阵。
- 不把 scheduler 调优细节全部暴露为稳定 API。
- 不引入新的工作流编排层。

## Decisions

### Decision 1: 抽象执行模型，但不移除 oneTBB 默认地位

**选择**: 允许后续实现通过 `ExecutionBackend`、`BatchSource`、`BatchSink` 等概念组织流水线，同时规定 oneTBB 继续作为默认后端。

**理由**: 这能让架构演进与当前高性能实现兼容，而不需要先付出一次性迁移成本。

### Decision 2: 公共扩展点必须留在 `include/fqtools/` 边界内

**选择**: 若执行抽象需要公开配置或工厂接口，新增能力 SHALL 继续落在公共头文件边界内，而不是暴露 `src/` 内部类型。

**理由**: 这保持现有 embeddable C++ surface 与 interface-implementation separation。

### Decision 3: backend 选择与调优默认是可选 schema

**选择**: 任何 backend 标识、调优旋钮或 telemetry 开关都应作为 additive、optional schema 引入。

**理由**: 默认路径仍需兼容现有 CLI/runtime 行为，不能让 execution abstraction 变成强制迁移。

### Architecture

```text
CLI / commands
      │
      ▼
Processing pipeline interface
      │
      ▼
Execution abstraction
  ├── BatchSource
  ├── BatchTransform stages
  └── BatchSink
      │
      ▼
Default backend: oneTBB
```

## Risks / Trade-offs

| 风险 | 缓解措施 |
| --- | --- |
| 抽象层过度设计 | 只规定 backend/source/sink 边界，不承诺过多稳定类型 |
| public API 被后端细节污染 | 只允许必要的工厂或配置扩展点进入公共头文件 |
| backend parity 验证成本增加 | 在 testing spec 中要求默认路径兼容与后端一致性测试 |

## Implementation Notes

- 与 memory policy、SIMD、advanced preprocessing 的实现计划解耦。
- 后续真正引入非 oneTBB backend 时，应以独立实现计划承接。
- 此设计优先约束边界，而不是规定某个具体执行器库。

---
*该设计文档只定义 execution abstraction 的边界，不直接替代具体实现设计。*
