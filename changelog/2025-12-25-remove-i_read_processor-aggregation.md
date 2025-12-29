# 2025-12-25 移除聚合头 i_read_processor（批次2）

## 背景
- `i_read_processor.h` 仅用于聚合 `read_mutator_interface.h` / `read_predicate_interface.h`，不承载真实接口定义。
- 聚合头易造成不必要的依赖耦合与命名噪声，本仓已统一采用 `*Interface` 命名与按需 include。

## 变更
- 删除聚合头（两处重复定义）：
  - `include/fqtools/pipeline/processing/i_read_processor.h`
  - `src/pipeline/processing/i_read_processor.h`
- 头文件依赖改为按需 include：
  - `include/fqtools/pipeline/processing/mutators/quality_trimmer.h` 改为直接 include `fqtools/pipeline/read_mutator_interface.h`
  - `include/fqtools/pipeline/processing/predicates/min_quality_predicate.h` 改为直接 include `fqtools/pipeline/read_predicate_interface.h`
- 语义化重命名实现文件：
  - `src/pipeline/processing/i_read_processor.cpp` -> `src/pipeline/processing/processing_statistics.cpp`
  - `ProcessingStatistics::toString()` 的实现改为直接 include `fqtools/pipeline/processing_pipeline_interface.h`
- 更新构建脚本引用：
  - `src/pipeline/CMakeLists.txt`
  - `src/modules/pipeline/CMakeLists.txt`

## 影响
- 仅影响内部构建与 include 关系，不改变对外接口与运行时行为。
