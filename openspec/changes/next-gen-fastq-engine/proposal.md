# Proposal: next-gen-fastq-engine

> **Status**: Draft
> **Created**: 2026-04-28
> **Author**: Copilot
> **Branch**: master
> **Commit**: not yet created
> **Related Baseline**:
> - [Product Spec](../../baseline/product/fastq-processing.md)
> - [Core Architecture](../../baseline/architecture/0001-core-architecture.md)
> - [Toolchain Policy](../../baseline/architecture/0002-toolchain-policy.md)
> - [Core API](../../baseline/api/core-api.md)
> - [Schema Spec](../../baseline/schemas/schema.md)
> - [Testing Spec](../../baseline/testing/test-strategy.md)

## Why Now

FastQTools 当前 baseline 明确把项目定义为 maintenance closeout 阶段的 FASTQ QC toolkit，并把扩展 public surface 视为非目标；但最新研究结论已经表明，项目最有价值的下一步不是继续收尾，而是把它重定位为高性能 FASTQ preprocessing engine，并以 execution abstraction、memory policy、advanced preprocessing 与 QC signatures 为后续独立子项目推进。

如果不先建立新的 OpenSpec 入口，后续所有实现都会与现有 baseline 的 closeout 叙事冲突，导致“代码在往前走，规范还在阻止它”的结构性漂移。

## Proposed Delta

本 proposal 只做第一步：允许项目从 closeout-only 叙事转向“聚焦但继续演进”的 advanced preprocessing engine 叙事，并为后续 4 个独立 proposal 打开入口。

### Baseline Touched

- [x] baseline/product
- [x] baseline/architecture
- [x] baseline/api
- [x] baseline/schemas
- [x] baseline/testing

### Delta Scope

1. 重定义产品目标：从“收敛中的 QC toolkit”调整为“聚焦的 FASTQ preprocessing engine”。
2. 允许新增 execution abstraction、pair-aware processing、structured outputs、signature modules 等后续扩展点。
3. 明确高级化的边界：继续排除 aligner、assembler、taxonomy platform 和 workflow runner。
4. 为后续子 proposal 预留顺序：execution-abstraction → memory-resource-policy → advanced-preprocessing → qc-signatures-and-screening。

### Non-Goals

- 本 proposal 不直接引入新的 C++ 运行时或新的 CLI 子命令。
- 本 proposal 不立即实现 SIMD、PMR、paired-end、UMI、k-mer/minimizer。
- 本 proposal 不把 FastQTools 扩展成通用比对、组装、分类或工作流平台。

## Compatibility / Risk

- **兼容性风险**: 中等。主要风险在于产品与 API 叙事会从 closeout 转向 active evolution，需要确保后续实现节奏与新叙事一致。
- **回滚方式**: 若后续决定放弃演进，可删除 `openspec/changes/next-gen-fastq-engine/`，并保留当前 baseline 不变。

## Verification

- [ ] `git diff --check`
- [ ] `rg -n "closeout|advanced preprocessing engine|Non-Goals" openspec/changes/next-gen-fastq-engine/proposal.md`

## Archive Gate

- [ ] 对应实现已落地（提交或推送完成）
- [ ] baseline 已更新，或已明确 `no baseline delta`
- [ ] 归档时将补充 commit SHA（如适用）

---
*此提案遵循 FastQTools 维护期 OpenSpec 工作流，并明确把后续工作拆分为多个独立子项目。*
