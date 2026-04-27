# Tasks: memory-resource-policy

> **Proposal**: [proposal.md](./proposal.md)
> **Design**: [design.md](./design.md)

## 0. 预检与依赖

- [ ] 0.1 完成 `git status --short --branch`
- [ ] 0.2 确认 `next-gen-fastq-engine` 已作为上游入口存在，并记录与 `execution-abstraction` 的协同关系
- [ ] 0.3 确认本 proposal 只处理 memory policy 规范，不直接承诺默认 allocator 切换

## 1. 建立变更包

- [ ] 1.1 创建 `openspec/changes/memory-resource-policy/` 并补齐 proposal / design / tasks / specs
- [ ] 1.2 在 proposal 中明确 PMR、worker-local pools 与 allocation telemetry 的范围边界
- [ ] 1.3 在 design 中定义策略层与观测边界
- [ ] 1.4 在 tasks 中记录 benchmark / sanitizer / regression 是后续实现 gate

## 2. 对齐规范增量

- [ ] 2.1 在 product spec delta 中明确资源可预测性与可观测性属于支持目标
- [ ] 2.2 在 architecture/api/schema/testing spec delta 中分别约束策略边界、公共扩展点、可选配置与验证矩阵
- [ ] 2.3 明确默认路径保持兼容，不进行隐式 allocator 切换
- [ ] 2.4 如评审指出 scope 漂移，同步修正 proposal / design / specs

## 3. 验证与记录

- [ ] 3.1 运行 `git diff --check`
- [ ] 3.2 运行 `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/memory-resource-policy/proposal.md openspec/changes/memory-resource-policy/design.md openspec/changes/memory-resource-policy/specs`
- [ ] 3.3 在 `changelog/2026-04-28-memory-resource-policy-proposal.md` 记录目标、影响范围、验证方式
- [ ] 3.4 完成一次 review，确认没有把实验性 allocator 误写成默认标准

## 4. 合并与衔接

- [ ] 4.1 改动已落地（提交/推送完成）
- [ ] 4.2 以本 proposal 为基础，编写 memory policy 的后续实现计划
- [ ] 4.3 等真实实现稳定落地后，再考虑归档本 proposal

---
*本 tasks 文件只负责建立 memory resource policy 的 OpenSpec 边界。*
