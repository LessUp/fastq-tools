# 2026-04-28 - advanced-preprocessing proposal

## 目标

为 FastQTools 的 advanced preprocessing 建立 OpenSpec change package，并落地第一段最小运行时切片，把 adapter trimming 与 polyG/polyX tail trimming 接入现有 `filter` 工作流。

## 主要改动

- 新增 `openspec/changes/advanced-preprocessing/proposal.md`
- 新增 `openspec/changes/advanced-preprocessing/design.md`
- 新增 `openspec/changes/advanced-preprocessing/tasks.md`
- 新增 `openspec/changes/advanced-preprocessing/specs/` 下 5 个最小 spec delta 文件
- 在 `include/fqtools/processing/mutators/quality_trimmer.h` / `src/processing/mutators/quality_trimmer.cpp` 落地 `PolyTailTrimmer`
- 在 `src/cli/commands/filter_command.cpp` 为 `filter` 接入 `--adapter-seq`、`--adapter-min-overlap`、`--adapter-max-mismatches`、`--trim-poly-g`、`--trim-poly-x`
- 为 preprocessing 运行时切片补齐 unit / integration / Python CLI e2e 测试

## 影响范围

- `openspec/changes/advanced-preprocessing/`
- `include/fqtools/processing/`
- `src/processing/`
- `src/cli/commands/`
- `tests/unit/processing/`
- `tests/integration/`
- `tests/e2e/`
- `changelog/`

## 验证方式

- `git diff --check`
- `rg -n "TBD|TODO|implement later|fill in details" openspec/changes/advanced-preprocessing/proposal.md openspec/changes/advanced-preprocessing/design.md openspec/changes/advanced-preprocessing/specs`
- `cmake --build --preset clang-debug --target FastQTools test_mutators test_processing test_integration_pipeline`
- `ctest --test-dir build/clang-debug -R '^(test_mutators|test_processing|test_integration_pipeline|e2e_python_cli)$' --output-on-failure`
- `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
