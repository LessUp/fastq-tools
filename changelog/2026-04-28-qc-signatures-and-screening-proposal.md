# 2026-04-28 - qc-signatures-and-screening proposal

## 目标

为 FastQTools 的 lightweight QC signatures / screening 建立 OpenSpec change package，把 k-mer/minimizer、duplicate estimation、contamination hints 与 structured side reports 纳入规范边界。

## 主要改动

- 新增 `openspec/changes/qc-signatures-and-screening/proposal.md`
- 新增 `openspec/changes/qc-signatures-and-screening/design.md`
- 新增 `openspec/changes/qc-signatures-and-screening/tasks.md`
- 新增 `openspec/changes/qc-signatures-and-screening/specs/` 下 5 个最小 spec delta 文件

## 影响范围

- `openspec/changes/qc-signatures-and-screening/`
- `changelog/`

## 验证方式

- `git diff --check`
- `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/qc-signatures-and-screening/proposal.md openspec/changes/qc-signatures-and-screening/design.md openspec/changes/qc-signatures-and-screening/specs`
- `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
