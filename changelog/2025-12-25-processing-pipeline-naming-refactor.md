# 2025-12-25 processing_pipeline 命名重构（MongoDB 风格）

- `src/pipeline/processing/processing_pipeline.{h,cpp}`：将 `SequentialProcessingPipeline` 的成员变量从 `m_` 前缀改为 `camelCase_` 后缀：
  - `m_input_path` -> `inputPath_`
  - `m_output_path` -> `outputPath_`
  - `m_config` -> `config_`
  - `m_mutators` -> `mutators_`
  - `m_predicates` -> `predicates_`

