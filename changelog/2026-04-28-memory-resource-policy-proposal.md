# 2026-04-28 - memory-resource-policy proposal

## 目标

为 FastQTools 的 memory resource policy 建立 OpenSpec change package，把 PMR、worker-local pools、allocation telemetry 与资源边界验证纳入规范。

## 主要改动

- 新增 `openspec/changes/memory-resource-policy/proposal.md`
- 新增 `openspec/changes/memory-resource-policy/design.md`
- 新增 `openspec/changes/memory-resource-policy/tasks.md`
- 新增 `openspec/changes/memory-resource-policy/specs/` 下 5 个最小 spec delta 文件

## 影响范围

- `openspec/changes/memory-resource-policy/`
- `changelog/`

## 验证方式

- `git diff --check`
- `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/memory-resource-policy/proposal.md openspec/changes/memory-resource-policy/design.md openspec/changes/memory-resource-policy/specs`
- `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
