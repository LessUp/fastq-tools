# 2025-12-25 predicates 命名重构（MongoDB 风格）

- `include/fqtools/pipeline/processing/predicates/min_quality_predicate.h`
- `src/pipeline/processing/predicates/min_quality_predicate.cpp`

变更：
- 成员变量从 `m_` 前缀改为 `camelCase_` 后缀（例如 `m_min_quality` -> `minQuality_`）。
- 私有 helper 方法改为 `camelCase`（例如 `calculate_average_quality` -> `calculateAverageQuality`）。
