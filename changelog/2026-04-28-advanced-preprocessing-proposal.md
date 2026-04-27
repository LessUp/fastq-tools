# 2026-04-28 - advanced-preprocessing proposal

## 目标

为 FastQTools 的 advanced preprocessing 建立 OpenSpec change package，把 adapter/primer、paired-end、UMI、polyG/polyX 与 low-complexity 等现代 FASTQ conditioning 能力纳入规范边界。

## 主要改动

- 新增 `openspec/changes/advanced-preprocessing/proposal.md`
- 新增 `openspec/changes/advanced-preprocessing/design.md`
- 新增 `openspec/changes/advanced-preprocessing/tasks.md`
- 新增 `openspec/changes/advanced-preprocessing/specs/` 下 5 个最小 spec delta 文件

## 影响范围

- `openspec/changes/advanced-preprocessing/`
- `changelog/`

## 验证方式

- `git diff --check`
- `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/advanced-preprocessing/proposal.md openspec/changes/advanced-preprocessing/design.md openspec/changes/advanced-preprocessing/specs`
- `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
