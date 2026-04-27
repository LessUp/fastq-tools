# Tasks: execution-abstraction

> **Proposal**: [proposal.md](./proposal.md)
> **Design**: [design.md](./design.md)

## 0. 预检与依赖

- [ ] 0.1 完成 `git status --short --branch`
- [ ] 0.2 确认 `next-gen-fastq-engine` 已作为上游入口存在
- [ ] 0.3 确认本 proposal 只处理执行抽象规范，不直接承诺多后端运行时代码

## 1. 建立变更包

- [ ] 1.1 创建 `openspec/changes/execution-abstraction/` 并补齐 proposal / design / tasks / specs
- [ ] 1.2 在 proposal 中明确 oneTBB 从“唯一叙事”变成“默认后端”
- [ ] 1.3 在 design 中定义 backend/source/sink abstraction 边界
- [ ] 1.4 在 tasks 中记录后续代码实现必须单独规划

## 2. 对齐规范增量

- [ ] 2.1 在 product spec delta 中明确执行模型可以升级，但默认用户体验保持稳定
- [ ] 2.2 在 architecture spec delta 中定义 execution abstraction 与 oneTBB-default policy
- [ ] 2.3 在 api/schema/testing spec delta 中分别约束公共扩展点、可选配置与 backend parity 验证
- [ ] 2.4 如评审指出 scope 漂移，同步修正 proposal / design / specs

## 3. 验证与记录

- [ ] 3.1 运行 `git diff --check`
- [ ] 3.2 运行 `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/execution-abstraction/proposal.md openspec/changes/execution-abstraction/design.md openspec/changes/execution-abstraction/specs`
- [ ] 3.3 在 `changelog/2026-04-28-execution-abstraction-proposal.md` 记录目标、影响范围、验证方式
- [ ] 3.4 完成一次 review，确认没有把范围扩大到 workflow orchestration

## 4. 合并与衔接

- [ ] 4.1 改动已落地（提交/推送完成）
- [ ] 4.2 以本 proposal 为基础，编写 execution abstraction 的后续实现计划
- [ ] 4.3 等真实实现稳定落地后，再考虑归档本 proposal

---
*本 tasks 文件只负责建立 execution abstraction 的 OpenSpec 边界。*
