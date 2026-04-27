# Design: qc-signatures-and-screening

> **Proposal**: [proposal.md](./proposal.md)
> **Last Updated**: 2026-04-28
> **Related Upstream Change**: [next-gen-fastq-engine](../next-gen-fastq-engine/design.md)

## Context

研究路线建议 FastQTools 在不走向平台化的前提下，增加 lightweight sequence intelligence：k-mer/minimizer signatures、duplicate estimation、small-panel contamination hints，以及 richer structured reports。这些能力本质上是 preprocessing/QC 的增强，不是替代 alignment 或 taxonomy 系统，也不要求与其他 follow-up proposal 形成硬依赖链。

## Goals / Non-Goals

### Goals
- 为 lightweight QC signatures 与 screening 建立范围边界。
- 为 structured side outputs 与 sidecar artifacts 预留规范入口。
- 保持当前文本报告与默认 CLI 路径兼容。

### Non-Goals
- 不把项目扩展成 classifier、aligner、assembler 或 workflow runner。
- 不承诺外部数据库驱动的大型分析平台能力。
- 不把所有 structured output 都宣称为长期稳定服务契约。

## Decisions

### Decision 1: sequence intelligence 必须保持 lightweight

**选择**: k-mer/minimizer、duplicate estimation 与 contamination hints 被定义为 lightweight QC/signature 能力，而不是完整分类或比对系统。

### Decision 2: structured outputs 作为 side reports 建模

**选择**: richer outputs 可以以 side reports 或 sidecar artifacts 进入范围，但默认文本报告行为必须继续存在。

### Decision 3: determinism 与 disabled-path compatibility 必须进测试矩阵

**选择**: signature 结果和 structured outputs 若进入实现，必须验证 deterministic behavior，并确保未启用新路径时现有行为兼容。

### Architecture

```text
Processed reads / batches
      │
      ├── existing text statistics
      └── optional lightweight signature modules
             ├── k-mer / minimizer sketch
             ├── duplicate estimation
             ├── contamination hints
             └── structured side reports
```

## Risks / Trade-offs

| 风险 | 缓解措施 |
| --- | --- |
| 轻量分析被误读为平台能力 | 在 proposal/spec 中强化 Non-Goals |
| structured output 被误写成稳定服务契约 | 规定 side reports 默认是可选扩展 |
| signature 算法引入不确定性 | 在 testing spec 中要求 deterministic validation |

## Implementation Notes

- richer reports 与 sidecar schema 应保持 additive、optional。
- contamination hints 应被描述为 hints，而不是 authoritative classification。
- 若未来需要更重的算法能力，应另起 proposal，不挤进本 change package。

---
*该设计文档只定义 lightweight signature/screening 边界，不直接替代具体算法设计。*
