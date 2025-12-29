# 2025-12-25 FastqBatch 成员命名重构（MongoDB 风格）

- `include/fqtools/io/fastq_io.h`：
  - `FastqBatch` 私有成员从 `m_` 前缀改为 `camelCase_` 后缀：
    - `m_buffer` -> `buffer_`
    - `m_records` -> `records_`
    - `m_remainder_offset` -> `remainderOffset_`
  - 不修改对外公开方法名（`buffer()`/`records()` 等保持不变），仅调整内部实现与成员命名一致性。

