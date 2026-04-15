# 2025-12-25 全项目命名规范重构（MongoDB 风格）

> 本文件合并自 16 个同日同主题的独立 changelog 条目。

## 概述

将整个代码库的命名约定统一为 MongoDB 风格：
- 函数/方法：`camelCase`
- 变量/参数：`camelCase`
- 成员变量：`camelCase_` 后缀
- 常量：`kConstantName`

## 1. CLI 模块

### CommandInterface 重命名（破坏性变更）
- `src/cli/commands/i_command.h` → `src/cli/commands/command_interface.h`
- `fq::cli::ICommand` → `fq::cli::CommandInterface`
- 同步更新：`filter_command.h`、`stat_command.h`、`main.cpp`、`CMakeLists.txt`

### FilterCommand 成员重构
- `src/cli/commands/filter_command.h`/`.cpp`：
  - `m_config` → `config_`、`m_pipeline` → `pipeline_`
  - 内部 Config 字段改为 `camelCase`

## 2. IO 模块

### FastqBatch 成员重构
- `include/fqtools/io/fastq_io.h`：
  - `m_buffer` → `buffer_`、`m_records` → `records_`、`m_remainder_offset` → `remainderOffset_`

### Reader/Writer PIMPL 重构
- `FastqReader`/`FastqWriter`：`m_impl` → `impl_`

### IO 模块内部成员重构
- `SharedBuffer`：`m_impl` → `impl_`
- `BufferedStream`：`m_stream` → `stream_`、`m_buffer` → `buffer_`、`m_buffer_size` → `bufferSize_` 等
- `IOMetrics`：`m_stats` → `stats_`

## 3. FASTQ 模块

### FqRecord / MutableFqRecord / FqBatchT / FileInferrer
- `src/modules/fastq/fastq.h`：
  - `FqRecord`：`m_buffer` → `buffer_`、`m_name_offset` → `nameOffset_` 等（7 个成员）
  - `MutableFqRecord`：`m_name` → `name_`、`m_sequence` → `sequence_`、`m_quality` → `quality_`
  - `FqBatchT`：`m_records` → `records_`
  - `FileInferrer`：`m_file_path` → `filePath_`

## 4. Error 模块
- `src/modules/error/error.{h,cpp}`：
  - `m_category` → `category_`、`m_severity` → `severity_`、`m_message` → `message_` 等（6 个成员）

## 5. Processing 模块

### Pipeline 成员重构
- `SequentialProcessingPipeline`：
  - `m_input_path` → `inputPath_`、`m_output_path` → `outputPath_`、`m_config` → `config_` 等

### Mutators 重构
- `QualityTrimmer`/`LengthTrimmer`/`AdapterTrimmer`：成员 `m_` → `camelCase_` 后缀
- 私有方法：`trim_five_prime` → `trimFivePrime` 等

### Predicates 重构
- `m_min_quality` → `minQuality_`
- `calculate_average_quality` → `calculateAverageQuality`

### 接口别名清理（I* → *Interface）
- 清理 `IReadMutator`/`IReadPredicate`/`IProcessingPipeline` 等遗留别名
- 受影响文件：`processing_pipeline_interface.h`、`i_read_processor.h`、mutators/predicates 头文件

### Public API 语义化重命名
- `setInput` → `setInputPath`、`setOutput` → `setOutputPath`、`setConfig` → `setProcessingConfig`
- `addMutator` → `addReadMutator`、`addPredicate` → `addReadPredicate`
- 工厂函数：`make_processing_pipeline` → `makeProcessingPipeline`

## 6. Statistics 模块

### 成员与常量重构
- `FastqStatisticCalculator`：`m_options` → `options_`
- 常量：`MAX_QUAL`/`MAX_BASE_NUM` → `kMaxQual`/`kMaxBaseNum`
- `m_qual_offset` → `qualOffset_`

### Public API 语义化重命名
- `StatisticOptions` 字段：`input_fastq` → `inputFastqPath`、`output_stat` → `outputStatPath`、`batch_size` → `batchSize` 等（9 个字段）
- 工厂函数：`make_statistic_calculator` → `makeStatisticCalculator`
- 移除旧别名 `IStatisticCalculator`

## 7. Options 字段统一

### ProcessingConfig
- `batch_size` → `batchSize`、`thread_count` → `threadCount`、`read_chunk_bytes` → `readChunkBytes` 等（8 个字段）

### FastqReaderOptions
- `read_chunk_bytes` → `readChunkBytes`、`zlib_buffer_bytes` → `zlibBufferBytes`、`max_buffer_bytes` → `maxBufferBytes`

### FastqWriterOptions
- `zlib_buffer_bytes` → `zlibBufferBytes`、`output_buffer_bytes` → `outputBufferBytes`
