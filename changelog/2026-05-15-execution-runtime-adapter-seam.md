# 目标

将 `src/processing/execution_runtime.*` 从多回调编排接口收敛为单一 `execute(...)` 入口，并把 `filter` / `stat` 两条命令路径迁移到新的运行时适配器接口。

# 主要改动

- 为内部 **FASTQ execution runtime** 新增 `ExecutionRuntimeRequest`、`ExecutionRuntimeOutcome` 与单入口 `execute(...)` 适配器接口。
- 移除旧的运行时测试注入表面（`setCustomReader` / `setCustomWriter` / `makeAdapterForTesting`），改为构造期注入自定义 I/O。
- 将 `ProcessingPipeline` 与 `FastqStatisticCalculator` 改为通过命令适配器驱动运行时。
- 用新的运行时单元测试覆盖批次顺序、批大小、提交字节数、参数校验与并行提交顺序。
- 补回 `ProcessingPipeline` 在自定义 reader 被消费后的显式 rerun 保护，并新增对应集成回归测试。

# 影响范围

- `src/processing/execution_runtime.h`
- `src/processing/execution_runtime.cpp`
- `src/processing/processing_pipeline.h`
- `src/processing/processing_pipeline.cpp`
- `src/statistics/fq_statistic.cpp`
- `tests/integration/test_pipeline_integration.cpp`
- `tests/unit/processing/test_execution_runtime.cpp`

# 验证方式

- `./scripts/core/lint format`
- `cmake --build --preset clang-debug --target test_processing test_statistics test_cli test_integration_pipeline`
- `ctest --test-dir build/clang-debug -R '^(test_processing|test_statistics|test_cli|test_integration_pipeline)$' --output-on-failure`
- `ctest --test-dir build/clang-debug -R '^(test_processing|test_cli|test_statistics|test_integration_pipeline|e2e_shell_cli|e2e_python_cli)$' --output-on-failure`
