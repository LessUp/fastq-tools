# Tasks: advanced-preprocessing

> **Proposal**: [proposal.md](./proposal.md)
> **Design**: [design.md](./design.md)

## 0. 预检与依赖

- [ ] 0.1 完成 `git status --short --branch`
- [ ] 0.2 确认 `next-gen-fastq-engine` 已作为上游入口存在，并记录与 `execution-abstraction`、`memory-resource-policy` 的协同关系
- [ ] 0.3 确认本 proposal 只处理高级 preprocessing 规范，不直接要求一次性实现所有算法

## 1. 建立变更包

- [ ] 1.1 创建 `openspec/changes/advanced-preprocessing/` 并补齐 proposal / design / tasks / specs
- [ ] 1.2 在 proposal 中明确 adapter/primer、paired-end、UMI、polyG/polyX、low-complexity、orphan/singleton policy 与 adapter wildcard/IUPAC 的范围
- [ ] 1.3 在 design 中定义模块化 preprocessing 边界
- [ ] 1.4 在 tasks 中记录后续实现必须分阶段推进并保持单端兼容

## 2. 对齐规范增量

- [ ] 2.1 在 product spec delta 中明确项目支持现代 FASTQ conditioning，但继续拒绝平台化
- [ ] 2.2 在 architecture/api/schema/testing spec delta 中分别约束模块边界、pair-aware 扩展点、可选配置、orphan/singleton policy、adapter wildcard/IUPAC 与测试矩阵
- [ ] 2.3 明确 UMI 只作为 preprocessing 能力进入范围，不强制绑定 dedup workflow
- [ ] 2.4 明确 richer structured side outputs 主要由后续 `qc-signatures-and-screening` 承接
- [ ] 2.5 如评审指出 scope 漂移，同步修正 proposal / design / specs

## 3. 验证与记录

- [ ] 3.1 运行 `git diff --check`
- [ ] 3.2 运行 `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/advanced-preprocessing/proposal.md openspec/changes/advanced-preprocessing/design.md openspec/changes/advanced-preprocessing/specs`
- [ ] 3.3 在 `changelog/2026-04-28-advanced-preprocessing-proposal.md` 记录目标、影响范围、验证方式
- [ ] 3.4 完成一次 review，确认没有滑向 aligner/assembler/workflow scope

## 4. 合并与衔接

- [ ] 4.1 改动已落地（提交/推送完成）
- [ ] 4.2 以本 proposal 为基础，编写 advanced preprocessing 的后续实现计划
- [ ] 4.3 等真实实现稳定落地后，再考虑归档本 proposal

---
*本 tasks 文件只负责建立 advanced preprocessing 的 OpenSpec 边界。*
