# 2025-12-25 processing pipeline public API 语义化重命名（MongoDB 风格）

- `ProcessingPipelineInterface` public API 由简写/非语义化命名统一为语义化的 `camelCase`：
  - `setInput` -> `setInputPath`
  - `setOutput` -> `setOutputPath`
  - `setConfig` -> `setProcessingConfig`
  - `addMutator` -> `addReadMutator`
  - `addPredicate` -> `addReadPredicate`
- processing pipeline 工厂函数命名统一为 `camelCase`：
  - `make_processing_pipeline` -> `makeProcessingPipeline`
  - `create_processing_pipeline` -> `createProcessingPipeline`
- 同步更新调用点：
  - `src/pipeline/processing/processing_pipeline.{h,cpp}`
  - `src/pipeline/factory.cpp`
  - `src/cli/commands/filter_command.cpp`
  - `tests/unit/modules/pipeline/test_pipeline_smoke.cpp`

