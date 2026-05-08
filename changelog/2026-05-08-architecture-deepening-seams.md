# 架构深化：运行时策略、过滤计划与统计报告集中化

## 目标

将 processing、CLI filter 组装和 statistics report 中仍然分散的浅层逻辑收拢到更深的内部模块里，在不改变当前公开 CLI / `ProcessingOptions` 使用方式的前提下提升 locality。

## 主要改动

- 新增 `src/processing/runtime_policy.*`，统一派生 processing 与 statistics 共用的运行时策略。
- 新增 `src/processing/pipeline_execution_plan.*`，集中决定串行/并行执行模式，避免 `ProcessingPipeline::run()` 内联判断扩散。
- 新增 `src/cli/filter_plan.*`，把 `filter` 命令的 predicate / mutator 组装从命令执行函数中抽离。
- 新增 `src/statistics/statistics_report.*`，把统计报告与 signature sidecar 的格式化/汇总逻辑从 `StatisticsWriter` 中抽离。
- 补充 CLI / processing / statistics 的单元测试，覆盖新内部模块的接口行为。

## 影响范围

- `src/processing/*`
- `src/cli/*`
- `src/statistics/*`
- `tests/unit/*`

## 验证方式

- `./scripts/core/lint format`
- `cmake --build --preset clang-debug`
- `ctest --test-dir build/clang-debug -R '^(test_processing|test_cli|test_statistics|test_integration_pipeline|e2e_shell_cli|e2e_python_cli)$' --output-on-failure`
