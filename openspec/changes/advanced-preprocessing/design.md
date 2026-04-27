# Design: advanced-preprocessing

> **Proposal**: [proposal.md](./proposal.md)
> **Last Updated**: 2026-04-28
> **Related Upstream Change**: [next-gen-fastq-engine](../next-gen-fastq-engine/design.md)

## Context

FastQTools 当前已经具备基础 predicates/mutators，但研究路线指出更高价值的下一阶段能力是现代 FASTQ conditioning：adapter/primer upgrade、paired-end synchronization、UMI extraction、polyG/polyX correction 和 low-complexity handling。

这些能力不应直接拼接成一堆互相独立的开关，而应在 preprocessing engine 内部形成可组合、可验证、可回滚的模块边界。

## Goals / Non-Goals

### Goals
- 为高级 preprocessing 模块建立统一边界。
- 保持单端路径与现有 `stat` / `filter` 默认体验兼容。
- 为 pair-aware、metadata-aware、module-aware API/schema 预留入口。

### Non-Goals
- 不引入比对、组装、分类或 workflow 编排。
- 不要求在一个 proposal 中覆盖所有 possible preprocessing 算法。
- 不绕过 execution abstraction 与 memory policy 直接堆叠功能。

## Decisions

### Decision 1: 高级 preprocessing 以模块化能力族推进

**选择**: adapter/primer、paired-end、UMI、polyG/polyX 和 low-complexity 被视为同一能力族下的模块，而不是多个互不相关的提案。

### Decision 2: pair-aware 扩展必须兼容单端路径并定义 orphan/singleton 策略

**选择**: 新能力可以增加 pair-aware record flow、mate consistency、orphan/singleton policy 与 metadata propagation，但默认单端路径必须保持兼容。

### Decision 3: adapter/primer 匹配必须显式支持策略边界

**选择**: adapter/primer 模块后续进入实现时，应覆盖 seed + bounded semiglobal fallback，以及 mismatch / overlap / wildcard/IUPAC 策略边界，而不是只保留模糊的“支持 adapter trimming”表述。

### Decision 4: UMI 仅作为 preprocessing 能力推进

**选择**: UMI 能力可以覆盖 extract/tag/filter，但不强制引入 dedup workflow，也不把 dedup 变成本 proposal 的必需范围。

### Decision 5: schema 与 testing 必须同步扩展

**选择**: 任何新 preprocessing 模块进入实现前，都必须同时定义可选 schema 和对应测试矩阵，而不是只改算法实现。

### Architecture

```text
Input reads / pairs
      │
      ▼
Preprocessing modules
  ├── adapter / primer
  ├── pair synchronization
  ├── UMI extraction
  ├── polyG / polyX handling
  └── low-complexity handling
      │
      ▼
Compatible outputs + statistics
```

## Risks / Trade-offs

| 风险 | 缓解措施 |
| --- | --- |
| 功能堆叠导致 CLI 失控 | 规定模块应优先通过可选 schema 与现有命令扩展接入 |
| pair-aware 破坏单端兼容 | 在 API/testing spec 中明确兼容 gate |
| UMI/metadata 生命周期复杂 | 保持 metadata 传播规则显式化并纳入测试 |

## Implementation Notes

- 后续真实实现可拆成更小执行计划，但都应归属于本 proposal 边界。
- structured side outputs 的 richer reporting 应与 `qc-signatures-and-screening` 协同，而不是在这里越界。
- 所有模块应遵守 zero-copy 与 explicit error handling 约束。

---
*该设计文档只定义 advanced preprocessing 的范围与结构，不直接替代具体算法实现设计。*
