# Tasks: final-state-convergence

> **Proposal**: [proposal.md](./proposal.md)
> **Design**: [design.md](./design.md)

## 0. 预检与工作区

- [x] 0.1 完成 `git status --short --branch`
- [x] 0.2 用户已明确授权直接在当前分支执行，不额外创建 branch / worktree
- [x] 0.3 确认当前工作区适合继续执行本任务

## 1. 规范同步

- [ ] 1.1 建立并固化 drift ledger，列出 benchmark 路径、命名空间、URL、版本锚点和 workflow 漂移项
- [ ] 1.2 重写受影响的 baseline 文档，使其只描述项目终态，而不是历史过渡状态
- [ ] 1.3 为 docs / workflow / GitHub metadata / AI 治理文档明确 baseline 回链关系
- [ ] 1.4 如设计偏移，持续同步 proposal / design / tasks

## 2. 仓库收敛实现

- [ ] 2.1 统一 benchmark 路径与相关引用，消除 `docs/benchmark-reports` 与 `docs/performance/*` 的漂移
- [ ] 2.2 统一仓库 URL、大小写、homepage、about 文案和版本锚点
- [ ] 2.3 清理 docs 主树、archive 与 changelog 噪音，保留高价值终态文档
- [ ] 2.4 重塑 Git Pages 首页与导航，形成项目落地页 + 精炼文档入口
- [ ] 2.5 精简 CI / release / benchmark / valgrind 工作流，只保留终态需要的自动化
- [ ] 2.6 重写 AGENTS / CLAUDE / Copilot / LSP / MCP 相关治理文档，封板 AI 工具链边界

## 3. 验证与记录

- [ ] 3.1 运行 `./scripts/core/lint format`
- [ ] 3.2 运行 `./scripts/core/build --preset clang-debug --no-lto`
- [ ] 3.3 运行 `./scripts/core/test --preset clang-debug --unit`
- [ ] 3.4 运行 `./scripts/core/test --preset clang-debug --integration`
- [ ] 3.5 运行 `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
- [ ] 3.6 在 `changelog/` 记录 baseline 引用、验证结果和风险点
- [ ] 3.7 对保留的 workflow / pages / about 配置做一次人工一致性复核

## 4. 交接与归档

- [ ] 4.1 输出 GLM 可直接执行的最终 Todo / gate / risk handoff
- [ ] 4.2 改动稳定落地后，将 proposal 归档到 `openspec/archive/`
- [ ] 4.3 归档时补充 commit SHA 与最终验证结果

---
*本任务单面向维护收尾期，核心原则是：先收敛事实来源，再做展示层和自动化层的减法。*
