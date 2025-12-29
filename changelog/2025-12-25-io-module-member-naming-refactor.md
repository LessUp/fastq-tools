# 2025-12-25 io 模块成员命名重构（MongoDB 风格）

- `src/modules/io/io.h`
  - `SharedBuffer` 私有成员从 `m_` 前缀改为 `camelCase_` 后缀：
    - `m_impl` -> `impl_`
  - `BufferedStream` 私有成员从 `m_` 前缀改为 `camelCase_` 后缀：
    - `m_stream` -> `stream_`
    - `m_buffer` -> `buffer_`
    - `m_buffer_size` -> `bufferSize_`
    - `m_buffer_pos` -> `bufferPos_`
    - `m_buffer_end` -> `bufferEnd_`
    - `m_bytes_processed` -> `bytesProcessed_`
  - `IOMetrics` 私有成员从 `m_` 前缀改为 `camelCase_` 后缀：
    - `m_stats` -> `stats_`

