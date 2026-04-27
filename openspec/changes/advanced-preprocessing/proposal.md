# Proposal: advanced-preprocessing

> **Status**: Draft
> **Created**: 2026-04-28
> **Author**: Copilot
> **Branch**: master
> **Commit**: not yet created
> **Related Upstream Change**: [next-gen-fastq-engine](../next-gen-fastq-engine/proposal.md)
> **Related Baseline**:
> - [Product Spec](../../baseline/product/fastq-processing.md)
> - [Core Architecture](../../baseline/architecture/0001-core-architecture.md)
> - [Core API](../../baseline/api/core-api.md)
> - [Schema Spec](../../baseline/schemas/schema.md)
> - [Testing Spec](../../baseline/testing/test-strategy.md)

## Why Now

作为 `next-gen-fastq-engine` 之后的独立子项目，advanced preprocessing 负责把 preprocessing 能力向现代 FASTQ conditioning 推进。它与 execution abstraction、memory policy 协同，但不要求和它们以硬依赖链一起落地。高价值方向包括 adapter/primer 升级、paired-end 同步、UMI extraction、polyG/polyX 处理与 low-complexity 过滤。

如果这些能力继续零散地以局部开关或单个 mutator 出现，产品边界、API 设计和测试策略会很快失去一致性。

## Proposed Delta

本 proposal 为 advanced preprocessing 建立边界，使未来能力扩张仍保持“聚焦 preprocessing engine”的定位。

### Baseline Touched

- [x] baseline/product
- [x] baseline/architecture
- [x] baseline/api
- [x] baseline/schemas
- [x] baseline/testing

### Delta Scope

1. 允许项目增加 adapter/primer、paired-end、UMI、polyG/polyX 与 low-complexity 等 preprocessing 模块，其中包括 orphan/singleton policy、adapter mismatch/overlap/wildcard/IUPAC 策略，以及 UMI extract/tag/filter。
2. 规定这些能力应以模块化、可选方式进入现有 CLI/runtime/API。
3. 允许增加 pair-aware 与 metadata-aware 的公共扩展点，但保持 zero-copy 约束。
4. 要求 schema 和测试策略同步覆盖 paired-end 路径、模块开关与兼容性保护。

### Non-Goals

- 本 proposal 不把项目扩展成 aligner、assembler、taxonomy platform 或 workflow runner。
- 本 proposal 不要求一次性上线所有 preprocessing 算法。
- 本 proposal 不以破坏单端路径兼容性为代价推进新能力。
- 本 proposal 不强制把 UMI 处理绑定到 dedup 工作流。

## Compatibility / Risk

- **兼容性风险**: 中高。pair-aware 与 metadata-aware 扩展若边界不清，容易侵入当前单端 API 与 CLI。
- **回滚方式**: 删除 `openspec/changes/advanced-preprocessing/`，继续维持当前较窄的 preprocessing baseline。

## Verification

- [ ] `git diff --check`
- [ ] `rg -n "paired-end|UMI|polyG|low-complexity|Non-Goals" openspec/changes/advanced-preprocessing/proposal.md`

## Archive Gate

- [ ] 对应实现已落地（提交或推送完成）
- [ ] baseline 已更新，或已明确 `no baseline delta`
- [ ] 归档时将补充 commit SHA（如适用）

---
*此提案把高级预处理合法化，但继续拒绝平台化扩张。*
