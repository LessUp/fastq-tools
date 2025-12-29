# 2025-12-25 options 字段命名统一为 camelCase（MongoDB 风格）

- `ProcessingConfig`（`include/fqtools/pipeline/processing_pipeline_interface.h`）字段从 `snake_case` 统一为 `camelCase`：
  - `batch_size` -> `batchSize`
  - `thread_count` -> `threadCount`
  - `read_chunk_bytes` -> `readChunkBytes`
  - `zlib_buffer_bytes` -> `zlibBufferBytes`
  - `writer_buffer_bytes` -> `writerBufferBytes`
  - `batch_capacity_bytes` -> `batchCapacityBytes`
  - `memory_limit_bytes` -> `memoryLimitBytes`
  - `max_in_flight_batches` -> `maxInFlightBatches`
- `FastqReaderOptions`（`include/fqtools/io/fastq_reader.h`）字段从 `snake_case` 统一为 `camelCase`：
  - `read_chunk_bytes` -> `readChunkBytes`
  - `zlib_buffer_bytes` -> `zlibBufferBytes`
  - `max_buffer_bytes` -> `maxBufferBytes`
- `FastqWriterOptions`（`include/fqtools/io/fastq_writer.h`）字段从 `snake_case` 统一为 `camelCase`：
  - `zlib_buffer_bytes` -> `zlibBufferBytes`
  - `output_buffer_bytes` -> `outputBufferBytes`
- 同步更新调用点（示例）：
  - `src/pipeline/processing/processing_pipeline.cpp`
  - `src/io/fastq_reader.cpp`
  - `src/io/fastq_writer.cpp`
  - `src/cli/commands/filter_command.cpp`
  - `src/pipeline/statistics/fq_statistic.cpp`

