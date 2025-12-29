# 2025-12-25 processing 接口别名清理（I* -> *Interface）

- 清理处理管线中遗留的 `I*` 类型别名与用法，统一使用 `*Interface` 命名：
  - `IReadMutator`/`IReadPredicate`/`IProcessingPipeline` 等不再保留。
- 受影响文件：
  - `include/fqtools/pipeline/processing_pipeline_interface.h`
  - `include/fqtools/pipeline/processing/i_read_processor.h`
  - `src/pipeline/processing/i_read_processor.h`
  - `include/fqtools/pipeline/processing/mutators/quality_trimmer.h`
  - `include/fqtools/pipeline/processing/predicates/min_quality_predicate.h`

