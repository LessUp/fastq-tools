# Proposal: memory-resource-policy

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

`next-gen-fastq-engine` 已经把继续演进的入口合法化。作为后续独立子项目，memory resource policy 的重点不是盲目替换全局 allocator，而是把 scratch memory、worker-local pools、allocation telemetry 与资源约束策略纳入规范。这样后续性能优化才能被描述、测试和回滚。

如果继续把内存策略视为实现细节，项目很难在不破坏 zero-copy 语义的情况下稳定引入 PMR、pool policy 与 allocator A/B 基准。

## Proposed Delta

本 proposal 为 memory resource policy 建立边界，要求相关配置与观测能力以可选、加法方式进入项目。

### Baseline Touched

- [x] baseline/product
- [x] baseline/architecture
- [x] baseline/api
- [x] baseline/schemas
- [x] baseline/testing

### Delta Scope

1. 允许 PMR scratch policy、worker-local pools 与 allocation telemetry 成为受支持的后续方向。
2. 规定默认路径仍需保持当前资源边界与行为兼容。
3. 允许公共 API 与 schema 以可选方式暴露 memory policy 配置和观测接口。
4. 要求 benchmark、sanitizer 与 memory-bound regression 一并进入验证矩阵。

### Non-Goals

- 本 proposal 不默认替换全局 allocator。
- 本 proposal 不把 jemalloc、mimalloc 或其他 allocator 直接承诺为默认依赖。
- 本 proposal 不改变 zero-copy ownership model。

## Compatibility / Risk

- **兼容性风险**: 中等。若内存策略接口设计过宽，容易把实验性 allocator 细节固化进稳定表面。
- **回滚方式**: 删除 `openspec/changes/memory-resource-policy/`，继续维持当前未显式建模的 memory policy baseline。

## Verification

- [ ] `git diff --check`
- [ ] `rg -n "PMR|worker-local|allocation telemetry|Non-Goals" openspec/changes/memory-resource-policy/proposal.md`

## Archive Gate

- [ ] 对应实现已落地（提交或推送完成）
- [ ] baseline 已更新，或已明确 `no baseline delta`
- [ ] 归档时将补充 commit SHA（如适用）

---
*此提案为 memory policy 合法化边界，不预设某个 allocator 一定成为默认。*
