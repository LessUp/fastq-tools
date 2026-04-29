# 2026-04-28 - qc-signatures-and-screening proposal

## 目标

为 FastQTools 的 lightweight QC signatures / screening 建立 OpenSpec change package，并落地第一段最小运行时切片，把 duplicate estimate 与 bounded head-kmer sidecar 接入现有 `stat` 工作流。

## 主要改动

- 新增 `openspec/changes/qc-signatures-and-screening/proposal.md`
- 新增 `openspec/changes/qc-signatures-and-screening/design.md`
- 新增 `openspec/changes/qc-signatures-and-screening/tasks.md`
- 新增 `openspec/changes/qc-signatures-and-screening/specs/` 下 5 个最小 spec delta 文件
- 在 `StatisticOptions` 与 `stat` CLI 中新增 `signatureReportPath`、`signatureKmerSize`、`maxReportedSignatures`、`duplicateEstimateSampleModulo`
- 在 `src/statistics/fq_statistic*.{h,cpp}` 与 `src/statistics/fq_statistic_worker*.{h,cpp}` 落地采样式 duplicate estimate、bounded head-kmer 聚合与可选 TSV sidecar
- 文本统计报告新增 `#DuplicateEstimate` / `#DuplicateEstimateRate`，并在启用 sidecar 时输出 `metric\tkey\tcount` 结构
- 为 QC signature 运行时切片补齐 unit / integration / Python CLI e2e 测试

## 影响范围

- `openspec/changes/qc-signatures-and-screening/`
- `include/fqtools/statistics/`
- `src/statistics/`
- `src/cli/commands/`
- `tests/unit/statistics/`
- `tests/integration/`
- `tests/e2e/`
- `changelog/`

## 验证方式

- `git diff --check`
- `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/qc-signatures-and-screening/proposal.md openspec/changes/qc-signatures-and-screening/design.md openspec/changes/qc-signatures-and-screening/specs`
- `cmake --build --preset clang-debug --target FastQTools test_statistics test_integration_pipeline`
- `ctest --test-dir build/clang-debug -R '^(test_statistics|test_integration_pipeline|e2e_python_cli)$' --output-on-failure`
- `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
