# 2025-10-21 全面命名与结构重构（批次 1-4）

> 本文件合并自 2 个同日同主题的独立 changelog 条目。

## 背景

去除 `I*` 风格接口命名，引入 `*Interface` 风格；将接口头前移到公共头树；实现类重命名；目录重组。

## 变更

### 批次 1：接口与公共头前移

- `IProcessingPipeline` → `ProcessingPipelineInterface`（兼容别名保留）
- `IStatisticCalculator` → `StatisticCalculatorInterface`
- `IStatistic` → `StatisticInterface`
- 新增 `include/fqtools/pipeline/` 和 `include/fqtools/statistics/` 公共接口头
- 新增 `make_processing_pipeline()` / `make_statistic_calculator()` 工厂函数
- `include/fqtools/fq.h` → `include/fqtools/app/app_info.h`

### 批次 2：实现类重命名

- `ProcessingPipeline` → `SequentialProcessingPipeline`
- `FqStatistic` → `FastqStatisticCalculator`
- `create_*` 工厂保留兼容，转发到 `make_*`

### 批次 3：目录重组

- `src/modules/pipeline/` → `src/pipeline/`
- CMake 更新：`src/CMakeLists.txt` 添加 `add_subdirectory(pipeline)`
- 示例与命令包含路径适配

### 批次 4：兼容清理（阶段性）

- 保留兼容期别名（`I*` 系列、`create_*` 工厂），待全量构建测试通过后清理

## 影响

- 外部包含路径 `<fqtools/processing_pipeline.h>`、`<fqtools/statistic_calculator.h>` 保持稳定
- 旧类型/工厂暂时仍可用，建议逐步迁移到新 API
