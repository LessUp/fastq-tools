---
description: FastQTools project rules for Cascade/Windsurf
---

# FastQTools Windsurf 规则

- `AGENTS.md` 是仓库主规则文件；本文件只保留 Windsurf/Cascade 需要的快捷入口。
- 规范与提案统一使用 `openspec/`：
  - 基线规范：`openspec/baseline/`
  - 变更提案：`openspec/changes/`
  - 规范索引：`openspec/README.md`
- 开发脚本统一使用 `scripts/core/`：
  - 构建：`./scripts/core/build`
  - 测试：`./scripts/core/test`
  - 代码质量：`./scripts/core/lint`
  - 依赖安装：`./scripts/core/install-deps`
- 开始任务前至少执行 `git status --short --branch`；`gh`、`git fetch`、`worktree` 检查按需使用，不再是固定门槛。
- 单人项目默认允许直接推送；只有在需要隔离高风险或并行任务时，才额外使用 branch / worktree。
- 不要再引用旧路径或归档文档作为当前事实来源，例如 `/specs`、`docs/decisions/*`、旧 `scripts/*.sh` 入口。
- 行为、公共 API、schema / 文件格式、架构 / 工具链 / 兼容性变更前，先阅读相关 `openspec/baseline/` 文档；需要改规范时，先在 `openspec/changes/` 中创建/更新提案。
- 若其他工具已占用当前改动，Windsurf/Cascade 切换为 review 或验证辅助，不重复实现同一补丁。
- 详细说明请链接到现有权威文档，而不是在这里重复：
  - `AGENTS.md`
  - `.github/copilot-instructions.md`
  - `scripts/core/README.md`
  - `docs/dev/index.md`
  - `openspec/baseline/architecture/0002-toolchain-policy.md`
