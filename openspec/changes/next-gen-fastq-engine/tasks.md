# Tasks: next-gen-fastq-engine

> **Proposal**: [proposal.md](./proposal.md)
> **Design**: [design.md](./design.md)

## 0. 预检与工作区

- [ ] 0.1 完成 `git status --short --branch`
- [ ] 0.2 确认继续在当前分支执行，不额外创建 worktree
- [ ] 0.3 确认本 proposal 只处理规格重定位，不直接修改运行时代码

## 1. 建立规格入口

- [ ] 1.1 创建 `openspec/changes/next-gen-fastq-engine/` 并补齐 proposal / design / tasks
- [ ] 1.2 在 proposal 中明确 closeout baseline 与未来演进方向的冲突点
- [ ] 1.3 在 design 中确定后续 4 个独立子 proposal 的边界与顺序
- [ ] 1.4 在 tasks 中记录后续工作必须分计划推进

## 2. 对齐后续实现前置条件

- [ ] 2.1 明确 baseline/product 将从 closeout toolkit 转向 preprocessing engine
- [ ] 2.2 明确 baseline/api 将允许 execution abstraction、pair-aware processing、structured outputs 等扩展点
- [ ] 2.3 明确 baseline/schemas、baseline/testing 需要在后续子项目中同步扩展
- [ ] 2.4 如评审导致方向偏移，同步更新 proposal / design / tasks

## 3. 验证与记录

- [ ] 3.1 运行 `git diff --check`
- [ ] 3.2 运行 `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/next-gen-fastq-engine`
- [ ] 3.3 在 `changelog/2026-04-28-next-gen-fastq-engine-proposal.md` 记录目标、影响范围、验证方式
- [ ] 3.4 完成一次人工或次级 AI review，确认 scope 没有滑向平台化

## 4. 合并与衔接

- [ ] 4.1 改动已落地（提交/推送完成）
- [ ] 4.2 以本 proposal 为基础，分别编写 4 份后续实现计划
- [ ] 4.3 等后续实现稳定落地后，再考虑归档本 proposal

---
*本 tasks 文件只负责建立入口与顺序；真正的代码实现将由后续独立计划承接。*
