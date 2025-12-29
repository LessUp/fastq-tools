# 2025-12-25 mutators 命名重构（MongoDB 风格）

- `include/fqtools/pipeline/processing/mutators/quality_trimmer.h`
- `src/pipeline/processing/mutators/quality_trimmer.cpp`

变更：
- `QualityTrimmer`/`LengthTrimmer`/`AdapterTrimmer` 成员变量从 `m_` 前缀改为 `camelCase_` 后缀。
- 私有 helper 方法从 `snake_case` 改为 `camelCase`（例如 `trim_five_prime` -> `trimFivePrime`）。

