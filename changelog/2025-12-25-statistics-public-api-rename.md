# 2025-12-25 statistics public API 语义化重命名（MongoDB 风格）

- `StatisticOptions` 字段从 `snake_case` 统一为语义化 `camelCase`：
  - `input_fastq` -> `inputFastqPath`
  - `output_stat` -> `outputStatPath`
  - `batch_size` -> `batchSize`
  - `thread_num` -> `threadCount`
  - `read_chunk_bytes` -> `readChunkBytes`
  - `zlib_buffer_bytes` -> `zlibBufferBytes`
  - `batch_capacity_bytes` -> `batchCapacityBytes`
  - `memory_limit_bytes` -> `memoryLimitBytes`
  - `max_in_flight_batches` -> `maxInFlightBatches`
- 统计计算器工厂函数命名统一为 `camelCase`：
  - `make_statistic_calculator` -> `makeStatisticCalculator`
  - `create_statistic_calculator` -> `createStatisticCalculator`
- 移除旧接口别名：
  - `IStatisticCalculator` 不再保留，具体实现直接继承 `StatisticCalculatorInterface`
- 同步更新调用点：
  - `include/fqtools/statistics/statistic_calculator_interface.h`
  - `src/pipeline/factory.cpp`
  - `src/pipeline/statistics/fq_statistic.{h,cpp}`
  - `src/cli/commands/stat_command.cpp`

