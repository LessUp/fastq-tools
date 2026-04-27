# Tasks: qc-signatures-and-screening

> **Proposal**: [proposal.md](./proposal.md)
> **Design**: [design.md](./design.md)

## 0. 预检与依赖

- [ ] 0.1 完成 `git status --short --branch`
- [ ] 0.2 确认 `next-gen-fastq-engine` 已作为上游入口存在，并记录与 `advanced-preprocessing` 的协同关系
- [ ] 0.3 确认本 proposal 只处理 lightweight signature/screening 规范，不直接承诺平台化能力

## 1. 建立变更包

- [ ] 1.1 创建 `openspec/changes/qc-signatures-and-screening/` 并补齐 proposal / design / tasks / specs
- [ ] 1.2 在 proposal 中明确 k-mer/minimizer、duplicate、contamination hints 与 structured reports 的范围
- [ ] 1.3 在 design 中定义 lightweight signature modules 与 side reports 边界
- [ ] 1.4 在 tasks 中记录后续实现必须保持 determinism 与 disabled-path compatibility

## 2. 对齐规范增量

- [ ] 2.1 在 product spec delta 中明确项目允许 lightweight sequence intelligence，但继续拒绝 classifier/platform 化
- [ ] 2.2 在 architecture/api/schema/testing spec delta 中分别约束模块边界、扩展点、structured schema 与测试矩阵
- [ ] 2.3 明确默认文本报告路径仍然保留
- [ ] 2.4 如评审指出 scope 漂移，同步修正 proposal / design / specs

## 3. 验证与记录

- [ ] 3.1 运行 `git diff --check`
- [ ] 3.2 运行 `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/qc-signatures-and-screening/proposal.md openspec/changes/qc-signatures-and-screening/design.md openspec/changes/qc-signatures-and-screening/specs`
- [ ] 3.3 在 `changelog/2026-04-28-qc-signatures-and-screening-proposal.md` 记录目标、影响范围、验证方式
- [ ] 3.4 完成一次 review，确认没有把 hints 误写成 authoritative classification

## 4. 合并与衔接

- [ ] 4.1 改动已落地（提交/推送完成）
- [ ] 4.2 以本 proposal 为基础，编写 qc signatures / screening 的后续实现计划
- [ ] 4.3 等真实实现稳定落地后，再考虑归档本 proposal

---
*本 tasks 文件只负责建立 lightweight QC signatures 的 OpenSpec 边界。*
