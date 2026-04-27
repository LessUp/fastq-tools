# 2026-04-28 - memory policy runtime slice

## 目标

为 memory-resource-policy 的第一段运行时代码建立最小可用表面：公开 `MemoryResourcePolicy` 与 telemetry 开关，并让 `filter` / `stat` 在不改变默认行为的前提下接受显式 `objectPool` 策略与可选 memory telemetry。

## 主要改动

- 新增公共头文件 `include/fqtools/processing/memory_resource_policy.h`
- 为 `ProcessingConfig`、`ProcessingStatistics` 与 `StatisticOptions` 增加 memory policy / telemetry 字段，默认仍为 `objectPool` 且 telemetry 关闭
- 让 processing pipeline 与 statistic calculator 显式识别 `objectPool` memory policy
- 当 telemetry 开启时，在 processing stats / `stat` 输出中记录 memory policy 与解析后的 `MaxInFlightBatches`
- 为 `filter` / `stat` CLI 新增 `--memory-policy objectPool` 与 `--allocation-telemetry`
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
