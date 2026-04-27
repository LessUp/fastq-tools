# 2026-04-28 - execution abstraction runtime slice

## 目标

为 execution abstraction 的第一段运行时代码建立最小可用表面：公开 `ExecutionBackend` 配置，并让 `filter` / `stat` 接受显式 `oneTbb` backend 选择，同时保持默认行为不变。

## 主要改动

- 新增公共头文件 `include/fqtools/processing/execution_backend.h`
- 为 `ProcessingConfig` 与 `StatisticOptions` 增加 `executionBackend` 字段，默认值为 `ExecutionBackend::OneTbb`
- 让 processing pipeline 与 statistic calculator 显式识别 oneTBB backend
- 为 `filter` / `stat` CLI 新增 `--execution-backend oneTbb` 选项
- 新增 unit / integration / Python CLI e2e 测试覆盖该切片

## 影响范围

- `include/fqtools/processing/`
- `include/fqtools/statistics/`
- `src/processing/`
- `src/statistics/`
- `src/cli/commands/`
- `tests/unit/processing/`
- `tests/integration/`
- `tests/e2e/`

## 验证方式

- `cmake --build --preset clang-debug --target FastQTools test_processing test_integration_pipeline`
- `ctest --test-dir build/clang-debug -R '^(test_processing|test_integration_pipeline)$' --output-on-failure`
- `FASTQTOOLS=/home/shane/dev/fastq-tools/build/clang-debug/FastQTools python3 -m unittest discover -s tests/e2e -p 'test_advanced_cli.py'`
