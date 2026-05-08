# 2026-04-28 - next-gen runtime truth alignment

## 目标

统一 next-gen runtime slices 的默认值与文档口径，确保 `stat` / `filter` 的实现、OpenSpec baseline、用户文档与 changelog 描述一致。

## 主要改动

- 将 `src/cli/commands/stat_command.cpp` 中 `--duplicate-sample-modulo` 的 CLI 默认值从 `20` 调整为 `1024`，与 `StatisticOptions` 保持一致
- 回写 `openspec/baseline/api/core-api.md`、`openspec/baseline/schemas/schema.md`、`openspec/baseline/testing/test-strategy.md`，补齐 execution backend、memory policy、advanced preprocessing、signature sidecar 真值
- 回写 `docs/guide/cli-reference*.md` 与 `docs/api/*.md`，移除陈旧的 JSON 输出描述并补齐 `ProcessingStatistics` / `StatisticOptions` 新字段
- 更新 `changelog/2026-04-28-advanced-preprocessing-proposal.md` 与 `changelog/2026-04-28-qc-signatures-and-screening-proposal.md`，记录 proposal 之后的 runtime 落地

## 影响范围

- `src/cli/commands/`
- `openspec/baseline/`
- `docs/guide/`
- `docs/api/`
- `changelog/`

## 验证方式

- `git diff --check`
- `cmake --build --preset clang-debug --target FastQTools test_statistics`
- `ctest --test-dir build/clang-debug -R '^(test_statistics|e2e_python_cli)$' --output-on-failure`
- `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
