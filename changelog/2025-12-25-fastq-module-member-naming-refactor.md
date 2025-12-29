# 2025-12-25 fastq 模块成员命名重构（MongoDB 风格）

- `src/modules/fastq/fastq.h`
  - 将 `FqRecord` 私有成员从 `m_` 前缀改为 `camelCase_` 后缀：
    - `m_buffer` -> `buffer_`
    - `m_name_offset` -> `nameOffset_`
    - `m_name_length` -> `nameLength_`
    - `m_sequence_offset` -> `sequenceOffset_`
    - `m_sequence_length` -> `sequenceLength_`
    - `m_quality_offset` -> `qualityOffset_`
    - `m_quality_length` -> `qualityLength_`
  - 将 `MutableFqRecord` 私有成员从 `m_` 前缀改为 `camelCase_` 后缀：
    - `m_name` -> `name_`
    - `m_sequence` -> `sequence_`
    - `m_quality` -> `quality_`
  - 将 `FqBatchT` 私有成员从 `m_` 前缀改为 `camelCase_` 后缀：
    - `m_records` -> `records_`
  - 将 `FileInferrer` 私有成员从 `m_` 前缀改为 `camelCase_` 后缀：
    - `m_file_path` -> `filePath_`

