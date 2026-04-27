# Proposal: qc-signatures-and-screening

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

作为 `next-gen-fastq-engine` 之后的独立子项目，FastQTools 可以继续向 lightweight sequence intelligence 迈进：k-mer/minimizer sketch、duplicate estimation、small-panel contamination hints，以及 structured side reports。它可以与 advanced preprocessing 协同，但不应被写成必须串行落地的硬依赖。

这些能力必须被严格限定为 lightweight QC/signature analysis；否则项目会滑向 classifier、aligner 或 workflow 平台。

## Proposed Delta

本 proposal 为 QC signatures 与 screening 建立边界，允许轻量序列智能能力进入范围，同时继续拒绝平台化扩张。

### Baseline Touched

- [x] baseline/product
- [x] baseline/architecture
- [x] baseline/api
- [x] baseline/schemas
- [x] baseline/testing

### Delta Scope

1. 允许项目增加 k-mer/minimizer signatures、duplicate estimation 与 contamination hints。
2. 允许为上述能力增加 structured side reports 或 sidecar outputs。
3. 要求这些能力保持 lightweight、可选，并与 preprocessing engine 紧耦合。
4. 要求 schema 与 testing 同步覆盖 structured outputs、determinism 与 disabled-path compatibility。

### Non-Goals

- 本 proposal 不把 FastQTools 扩展成 taxonomy classifier、read mapper 或 assembly platform。
- 本 proposal 不承诺大规模参考数据库或远程服务依赖。
- 本 proposal 不把 structured outputs 上升为服务化外部契约。

## Compatibility / Risk

- **兼容性风险**: 中等。structured outputs 与 signature hints 若边界不清，容易把轻量分析误写成平台级 contract。
- **回滚方式**: 删除 `openspec/changes/qc-signatures-and-screening/`，继续维持当前较窄的 QC/report baseline。

## Verification

- [ ] `git diff --check`
- [ ] `rg -n "k-mer|minimizer|structured|contamination|Non-Goals" openspec/changes/qc-signatures-and-screening/proposal.md`

## Archive Gate

- [ ] 对应实现已落地（提交或推送完成）
- [ ] baseline 已更新，或已明确 `no baseline delta`
- [ ] 归档时将补充 commit SHA（如适用）

---
*此提案允许 lightweight sequence intelligence，但继续拒绝 classifier / platform 化。*
