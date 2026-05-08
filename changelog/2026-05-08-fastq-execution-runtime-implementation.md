# FASTQ execution runtime 落地

## 目标

把 `filter` 与 `stat` 的共享执行编排收敛到内部 `ExecutionRuntime`，提升 locality 与可测试性。

## 主要改动

- 新增 `src/processing/execution_runtime.*`，统一批次遍历、适配器 I/O、运行时策略与并行执行入口
- `ProcessingPipeline` 改为通过 `ExecutionRuntime` 执行 batch 处理、提交与结果归并
- `FastqStatisticCalculator` 改为通过 `ExecutionRuntime` 执行 batch 统计与聚合
- 新增 `tests/unit/processing/test_execution_runtime.cpp`，覆盖接缝契约、懒初始化、自定义 I/O 与重复运行行为
- 新增 integration 回归，覆盖 pipeline modifiedReads 统计与 statistic 跨 batch 聚合

## 影响范围

- `src/processing/*`
- `src/statistics/*`
- `tests/unit/processing/*`
- `tests/integration/*`

## 验证方式

- `./scripts/core/lint format`
- `cmake --build build/clang-debug --target FastQTools test_processing test_statistics test_integration_pipeline`
- `ctest --test-dir build/clang-debug -R '^(test_processing|test_statistics|test_integration_pipeline|e2e_shell_cli|e2e_python_cli)$' --output-on-failure`

## 备注

- no baseline delta
