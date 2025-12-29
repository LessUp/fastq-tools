# 2025-12-25 statistics 模块命名重构（MongoDB 风格）

- `src/pipeline/statistics/fq_statistic.{h,cpp}`：
  - `FastqStatisticCalculator` 成员：`m_options` -> `options_`
- `src/pipeline/statistics/fq_statistic_worker.{h,cpp}`：
  - 常量：`MAX_QUAL`/`MAX_BASE_NUM` -> `kMaxQual`/`kMaxBaseNum`
  - 成员：`m_qual_offset` -> `qualOffset_`
- `src/pipeline/statistics/fq_statistic.cpp`：同步使用 `kMaxQual/kMaxBaseNum`。
