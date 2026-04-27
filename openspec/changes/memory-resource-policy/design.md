# Design: memory-resource-policy

> **Proposal**: [proposal.md](./proposal.md)
> **Last Updated**: 2026-04-28
> **Related Upstream Change**: [next-gen-fastq-engine](../next-gen-fastq-engine/design.md)

## Context

FastQTools 当前已经具备 zero-copy batch/record 模型和对象池，但 baseline 仍缺少对 scratch memory、worker-local reuse、allocation telemetry 与 allocator benchmark policy 的规范表达。随着 execution abstraction 被引入，这些资源策略需要从“隐式优化”变成“显式可验证边界”。

## Goals / Non-Goals

### Goals
- 为 PMR、worker-local pools 与 telemetry 建立规范入口。
- 保持默认路径与当前 zero-copy 生命周期兼容。
- 让资源约束、观测与 benchmark 成为后续实现的显式验收项。

### Non-Goals
- 不指定单一 allocator 成为唯一标准。
- 不要求把所有内存策略暴露成复杂 CLI。
- 不引入 silent fallback 或宽泛错误吞没。

## Decisions

### Decision 1: memory policy 以可选策略层形式建模

**选择**: 以后续 proposal/implementation 的方式支持 PMR scratch policy、worker-local pools 和 allocation telemetry，但这些能力默认是可选策略，不是强制迁移。

### Decision 2: allocator 选择与 telemetry 分开建模

**选择**: allocator backend、scratch resource、pool reuse 和 telemetry 开关可以一起协同，但不应在规范中被混成同一个固定实现。

### Decision 3: 验证矩阵必须包含 benchmark 与 sanitizer

**选择**: memory policy 不是只看编译通过；后续实现必须验证 memory-bound regression、allocator parity、benchmark 结果和 sanitizer 安全性。

### Architecture

```text
Execution abstraction
      │
      ▼
Memory resource policy
  ├── scratch resource policy
  ├── worker-local pool policy
  ├── allocation telemetry
  └── optional allocator backend hooks
```

## Risks / Trade-offs

| 风险 | 缓解措施 |
| --- | --- |
| allocator 方案过早定死 | 规范只允许策略层与观测入口，不指定唯一默认实现 |
| telemetry 增加运行时开销 | 规定 telemetry 默认可关闭 |
| 资源优化破坏生命周期规则 | 在 API/testing spec 中明确 zero-copy safety 不变 |

## Implementation Notes

- allocator A/B 与 benchmark matrix 应以后续实现计划推进。
- memory policy 与 execution abstraction 协同，但不应回退为 oneTBB-specific 优化集合。
- schema 扩展优先使用 additive、optional 字段。

---
*该设计文档只定义 memory policy 边界，不等同于具体 allocator 集成计划。*
