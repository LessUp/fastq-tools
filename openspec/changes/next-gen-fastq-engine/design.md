# Design: next-gen-fastq-engine

> **Proposal**: [proposal.md](./proposal.md)
> **Last Updated**: 2026-04-28
> **Optional**: 本设计用于界定 FastQTools 从 closeout toolkit 向 preprocessing engine 重定位时的边界与顺序

## Context

FastQTools 当前实现已经具备零拷贝 `FastqBatch` / `FastqRecord`、oneTBB pipeline、对象池与 embeddable C++ API，但 baseline 仍将项目描述为 maintenance closeout 阶段的有限 QC toolkit。研究结论表明，真正高价值的下一阶段能力包括 execution abstraction、SIMD hot kernels、PMR/worker-local memory policy、以及 adapter/primer、paired-end、UMI、signature/screen 等高级 preprocessing 模块。

在不先改写产品边界的前提下，后续实现会持续与现有 baseline 冲突。

## Goals / Non-Goals

### Goals
- 允许项目从 closeout 叙事转向 active evolution 叙事。
- 定义“什么属于 FastQTools 下一阶段能力，什么仍然不属于”。
- 规定后续子 proposal 的顺序，避免一次性大改。

### Non-Goals
- 本设计不直接实现新的运行时代码。
- 本设计不要求一次性改写所有 baseline。
- 本设计不允许项目滑向 aligner、assembler、taxonomy platform 或 workflow runner。

## Decisions

### Decision 1: 先重开产品边界，再开始代码级增强

**选择**: 先建立 `next-gen-fastq-engine` proposal，再分别推进 execution-abstraction、memory-resource-policy、advanced-preprocessing、qc-signatures-and-screening。

**理由**: 当前 closeout baseline 会阻止后续代码演进；先重开边界可以避免实现与规范继续背离。

**替代方案**:
1. 直接开始写 execution abstraction 代码 - 被拒绝，因为会与现有 baseline 冲突。
2. 一次性把所有新方向写进单个 mega proposal - 被拒绝，因为超出维护期 OpenSpec 的最小交付粒度。

### Decision 2: 保持“小而强的 preprocessing engine”，拒绝平台化

**选择**: 允许 execution abstraction、pair-aware processing、structured outputs、signature modules 等增强，但继续排除 mapping、assembly、taxonomy platform 与 workflow orchestration。

**理由**: FastQTools 的优势是路径短、性能高、可嵌入，不应在高级化过程中丢失聚焦。

**替代方案**:
1. 扩成通用生信平台 - 被拒绝，因为范围失控且维护成本过高。
2. 完全停留在 closeout toolkit - 被拒绝，因为会锁死最有价值的演进机会。

### Decision 3: 后续实现按四个独立子项目落地

**选择**:
1. `execution-abstraction`
2. `memory-resource-policy`
3. `advanced-preprocessing`
4. `qc-signatures-and-screening`

**理由**: 这四个方向彼此耦合，但可以独立评审、独立验证、独立回滚。

**替代方案**:
1. 合并成两个大项目 - 被拒绝，因为 review 面过宽。
2. 拆得更细（每个算法单独 proposal） - 被拒绝，因为管理成本过高。

### Architecture

```text
Current baseline
  closeout QC toolkit
        │
        ▼
next-gen-fastq-engine proposal
        │
        ├── execution-abstraction
        ├── memory-resource-policy
        ├── advanced-preprocessing
        └── qc-signatures-and-screening
```

## Risks / Trade-offs

| 风险 | 缓解措施 |
|-----|---------|
| 产品叙事先行，但代码尚未跟上 | 在 tasks.md 中明确后续子 proposal 顺序与 gate |
| 范围重新打开后再次失控 | 在 proposal 和 design 中强化 Non-Goals |
| reviewer 认为这是 roadmap 不是 proposal | 把本变更限定为“合法化入口”，不直接展开实现细节 |

## Implementation Notes

- 使用已有研究报告作为事实来源，但不要把外部研究原文整段复制到 proposal/design。
- 保持与 `final-state-convergence` 相同的 proposal/design/tasks 布局，减少仓库内风格分裂。
- 所有 follow-up 工作都应引用本 design，而不是绕过它直接改 baseline。

---
*该设计文档只负责为下一阶段演进建立边界与顺序，不直接替代后续实现设计。*
