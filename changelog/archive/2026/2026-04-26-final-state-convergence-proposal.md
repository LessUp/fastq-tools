# 2026-04-26 收尾重构提案落地

## 目标

为 FastQTools 的“最终完结态”收尾工程建立正式的 OpenSpec proposal / design / tasks 入口，确保后续对 baseline、文档、CI/CD、Pages、GitHub metadata 和 AI 治理文档的重构都有统一边界和任务索引。

## 主要改动

1. 在 `openspec/changes/final-state-convergence/` 下新增 proposal、design、tasks 三份文档。
2. 将本轮代码库审查中确认的关键漂移项收敛为正式变更范围，包括 benchmark 路径、命名空间、仓库 URL、工具链声明、workflow 复杂度和 AI 治理边界。
3. 明确本轮收尾工程采用“完结优先、删减优先、统一优先”的治理策略，并约束后续变更必须回链 baseline。

## 影响范围

- `openspec/changes/final-state-convergence/proposal.md`
- `openspec/changes/final-state-convergence/design.md`
- `openspec/changes/final-state-convergence/tasks.md`
- `changelog/2026-04-26-final-state-convergence-proposal.md`

## 验证方式

- 文档级变更，后续将在对应实现阶段执行 `./scripts/core/build --preset clang-debug --no-lto`、`./scripts/core/test --preset clang-debug --unit`、`./scripts/core/test --preset clang-debug --integration` 与 `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`。
- 当前记录的作用是为后续大规模收尾整改建立规范入口，属于 proposal 建档阶段。
