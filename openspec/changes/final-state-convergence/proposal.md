# Proposal: final-state-convergence

> **Status**: Draft
> **Created**: 2026-04-26
> **Author**: Copilot
> **Branch**: master（经用户明确授权，直接在当前分支收尾）
> **Commit**: not yet created
> **Related Baseline**:
> - [Product Spec](../../baseline/product/fastq-processing.md)
> - [Core Architecture](../../baseline/architecture/0001-core-architecture.md)
> - [Toolchain Policy](../../baseline/architecture/0002-toolchain-policy.md)
> - [Benchmark System](../../baseline/architecture/0003-benchmark-system.md)
> - [Core API](../../baseline/api/core-api.md)
> - [Schema Spec](../../baseline/schemas/schema.md)
> - [Testing Spec](../../baseline/testing/test-strategy.md)

## Why Now

FastQTools 已经进入明显的维护收尾期：核心命令、文档站点、OpenSpec、CI/CD、AI 治理与本地工具链都已存在，但事实来源仍未完全收敛。当前仓库最主要的问题不再是“功能不足”，而是“多个系统对项目现状说出了不完全一致的话”。

本轮审查已确认以下高优先级漂移：

1. benchmark 路径在 OpenSpec、workflow、工具脚本、开发文档之间不一致；
2. `fq::statistic` / `fq::statistics` 命名存在规范-实现漂移；
3. 仓库 URL、大小写、GitHub metadata 文案、主页链接存在多处不一致；
4. toolchain baseline 对外宣称 GCC 15 / Clang 21 统一，而当前本地可验证链路仍是 Clang 18 + libc++；
5. CI / release / benchmark / valgrind 仍保留较重维护面，不符合“最终完结态”的最小维护成本目标；
6. AI 协作文档已经形成雏形，但仍存在重复、边界不够收敛的问题。

如果不先做一次激进收敛，后续无论继续修 bug、更新文档、还是交给 GLM 模型接手，都会再次遇到“规范可信度不足、资产过载、流程不清”的同类问题。

## Proposed Delta

本 proposal 只做一件事：把仓库从“功能大体完整但治理层仍漂移”的状态，收敛到“可随时归档的稳定完成态”。

### Baseline Touched

- [x] baseline/product
- [x] baseline/architecture
- [x] baseline/api
- [x] baseline/schemas
- [x] baseline/testing

### Delta Scope

1. **重切 baseline**
   - 只保留与项目终态直接相关、可长期维护的规范。
   - 删除或归档历史包袱、失效路径、重复叙事和无验证价值的声明。

2. **统一事实来源**
   - 统一 benchmark 存储路径、仓库 URL、命名空间表述、版本锚点、工具链描述。
   - 让 docs / spec / workflow / scripts / metadata 对项目现状给出同一份答案。

3. **精简工程化表面**
   - 压缩 GitHub Actions 到“必要而可信”的最小集合。
   - 重新论证 release、benchmark、valgrind 等长尾自动化是否保留、手动化还是归档。

4. **封板 AI 与本地工具链治理**
   - 重写并去重 AGENTS / CLAUDE / Copilot 指令。
   - 固化 clangd / preset / compile_commands / MCP / CLI skills 的分工。

5. **产出交接包**
   - 为 GLM 模型提供低上下文、可分派、可验证的最终 Todo 与风险边界。

### Non-Goals

- 不新增新的 CLI 子命令或业务能力。
- 不扩展新的平台矩阵或分发渠道。
- 不引入新的重量级基础设施（例如 repo-specific MCP、本地额外索引系统、复杂分支模型）。
- 不为了“看起来完整”而保留已经失去维护价值的历史资产。

## Compatibility / Risk

- **兼容性风险**: 中等。主要来自文档删除/归档、路径统一、CI 和 release 策略收敛，以及 toolchain 对外声明的重新校准。
- **回滚方式**: 所有变更均以仓库内文件与 GitHub 仓库设置为主，可通过 git 提交回退；远程 metadata/workflow 的回滚则通过 `gh` 或再次提交 workflow 恢复。
- **主要风险**:
  - 误删仍被引用的文档或历史资产；
  - baseline 更新不完整，导致 spec 与实现再次失配；
  - CI 收缩过度，失去必要质量门禁；
  - 远程仓库描述与本地 docs 页面未同步闭环。

## Verification

- [ ] `./scripts/core/build --preset clang-debug --no-lto`
- [ ] `./scripts/core/test --preset clang-debug --unit`
- [ ] `./scripts/core/test --preset clang-debug --integration`
- [ ] `./scripts/core/lint format`
- [ ] `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
- [ ] 关键 workflow 触发条件与保留策略完成人工复核
- [ ] GitHub about / Pages / topics 与仓库首页叙事完成一致性复核

## Archive Gate

- [ ] 对应实现已落地（提交或推送完成）
- [ ] baseline 已更新，或已明确 `no baseline delta`
- [ ] 归档时将补充 commit SHA（如适用）

---
*此提案遵循 FastQTools 维护期 OpenSpec 工作流，并以“完结优先、删减优先、统一优先”为执行原则。*
