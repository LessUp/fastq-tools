# Tasks: <name>

> **Proposal**: [proposal.md](./proposal.md)
> **Design**: [design.md](./design.md)（如适用）

## 0. 预检与工作区

- [ ] 0.1 完成 `git status --short --branch`
- [ ] 0.2 如有需要，创建独立 branch / worktree
- [ ] 0.3 确认当前工作区适合继续执行本任务

## 1. 规范同步

- [ ] 1.1 更新受影响的 baseline 文档
- [ ] 1.2 如设计发生偏移，同步更新 proposal / design
- [ ] 1.3 记录 `no baseline delta`（如适用）

## 2. 实现

- [ ] 2.1 <最小必要实现 1>
- [ ] 2.2 <最小必要实现 2>

## 3. 验证与提交

- [ ] 3.1 运行与改动直接相关的最小验证
- [ ] 3.2 在提交说明或变更记录中记录 baseline 引用、验证结果、风险点
- [ ] 3.3 如有需要，补充一次人工或次级 AI 的 diff review

## 4. 合并与归档

- [ ] 4.1 改动已落地（提交/推送完成）
- [ ] 4.2 将 proposal 归档到 `openspec/archive/`
- [ ] 4.3 在归档中补充 commit SHA（如适用）

---
*仅在改动稳定落地后归档；未落地 proposal 保留在 `openspec/changes/`*
