# Proposal: execution-abstraction

> **Status**: Draft
> **Created**: 2026-04-28
> **Author**: Copilot
> **Branch**: master
> **Commit**: not yet created
> **Depends On**: [next-gen-fastq-engine](../next-gen-fastq-engine/proposal.md)
> **Related Baseline**:
> - [Product Spec](../../baseline/product/fastq-processing.md)
> - [Core Architecture](../../baseline/architecture/0001-core-architecture.md)
> - [Core API](../../baseline/api/core-api.md)
> - [Schema Spec](../../baseline/schemas/schema.md)
> - [Testing Spec](../../baseline/testing/test-strategy.md)

## Why Now

`next-gen-fastq-engine` 已经把 FastQTools 合法化为可继续演进的 preprocessing engine。下一步最核心的基础工作，是把当前几乎等同于 oneTBB pipeline 实现细节的执行路径，提升为明确的 execution abstraction，同时保留 oneTBB 作为默认后端。

如果不先做这层规格拆分，后续 SIMD、memory policy、pair-aware modules 与 structured outputs 都会继续耦合在单一运行时假设里，导致实现扩展点与 baseline 无法对齐。

## Proposed Delta

本 proposal 只建立 execution abstraction 的规范边界，不直接要求一次性引入多套生产后端。

### Baseline Touched

- [x] baseline/product
- [x] baseline/architecture
- [x] baseline/api
- [x] baseline/schemas
- [x] baseline/testing

### Delta Scope

1. 把批处理执行模型表述为 backend/source/sink abstraction，而不是 oneTBB 专有实现细节。
2. 明确 Intel oneTBB 仍然是默认且受支持的执行后端。
3. 允许未来在 `include/fqtools/` 公共边界内增加 execution-oriented extension points。
4. 要求 runtime/schema 以可选、加法方式为 backend 选择和调优预留空间。
5. 要求后续实现验证默认路径兼容性、backend parity 与 zero-copy 生命周期安全。

### Non-Goals

- 本 proposal 不默认切换到 stdexec/P2300 或其他新后端。
- 本 proposal 不直接实现 SIMD、allocator policy 或 paired-end preprocessing。
- 本 proposal 不扩大 CLI 为任务编排器或工作流平台。

## Compatibility / Risk

- **兼容性风险**: 中等。抽象层引入后若边界不清，容易把内部实现细节泄漏到 public API。
- **回滚方式**: 删除 `openspec/changes/execution-abstraction/`，继续维持当前 oneTBB-specific baseline 叙事。

## Verification

- [ ] `git diff --check`
- [ ] `rg -n "ExecutionBackend|oneTBB|backend parity|Non-Goals" openspec/changes/execution-abstraction/proposal.md`

## Archive Gate

- [ ] 对应实现已落地（提交或推送完成）
- [ ] baseline 已更新，或已明确 `no baseline delta`
- [ ] 归档时将补充 commit SHA（如适用）

---
*此提案把运行时抽象合法化，但默认后端仍然保持 oneTBB。*
