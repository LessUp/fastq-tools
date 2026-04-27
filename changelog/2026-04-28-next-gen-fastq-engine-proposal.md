# 2026-04-28 - next-gen-fastq-engine proposal

## 目标

为 FastQTools 下一阶段演进建立 OpenSpec 合法入口，把项目从 closeout-only QC toolkit 叙事推进到“聚焦但继续演进”的 FASTQ preprocessing engine 叙事。

## 主要改动

- 新增 `openspec/changes/next-gen-fastq-engine/proposal.md`
- 新增 `openspec/changes/next-gen-fastq-engine/design.md`
- 新增 `openspec/changes/next-gen-fastq-engine/tasks.md`
- 明确后续 4 个独立子项目：execution-abstraction、memory-resource-policy、advanced-preprocessing、qc-signatures-and-screening

## 影响范围

- `openspec/changes/next-gen-fastq-engine/`
- `changelog/`

## 验证方式

- `git diff --check`
- `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/next-gen-fastq-engine`
- `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
