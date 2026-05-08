# 架构深化重构

## 目标

实施 4 个深化重构，将浅模块转变为深模块，提升可测试性和可维护性。

## 主要改动

### 1. CLI 共享参数提取 + 配置分层

**新增**：
- `include/fqtools/processing/processing_options.h` - 用户可见的 `ProcessingOptions`、性能预设 `ProcessingProfile`、内部配置 `InternalConfig`
- `src/processing/internal_config.cpp` - 预设映射实现
- `src/cli/common_options.h/cpp` - 共享 CLI 参数定义和解析

**修改**：
- `include/fqtools/processing/processing_pipeline_interface.h` - 使用 `ProcessingOptions` 替代 `ProcessingConfig`
- `include/fqtools/statistics/statistic_calculator_interface.h` - `StatisticOptions` 嵌入 `ProcessingOptions`
- `src/cli/commands/stat_command.cpp` - 使用 `CommonCliOptions`
- `src/cli/commands/filter_command.cpp` - 使用 `CommonCliOptions`
- `src/processing/processing_pipeline.cpp` - 使用 `InternalConfig`

### 2. 移除假接缝

**删除**：
- `include/fqtools/processing/execution_backend.h`
- `include/fqtools/processing/memory_resource_policy.h`
- `src/cli/commands/execution_backend_option.h`
- `src/cli/commands/memory_resource_policy_option.h`

**简化**：
- `ProcessingStatistics` - 移除 `memoryResourcePolicy` 和 `resolvedMaxInFlightBatches`
- `StatisticsWriterOptions` - 移除内存遥测相关字段

### 3. API 变更

**删除的公共接口**：
- `ProcessingConfig` 结构体 → 替换为 `ProcessingOptions`
- `ExecutionBackend` 枚举 → 删除
- `MemoryResourcePolicy` 枚举 → 删除

**新增的公共接口**：
- `ProcessingOptions` - 用户可见的处理选项
- `ProcessingProfile` - 性能预设枚举（Default/LowMemory/HighThroughput）

### 4. CLI 参数变更

**删除的参数**：
- `--execution-backend`
- `--memory-policy`
- `--read-chunk-bytes`
- `--batch-capacity-bytes`
- `--zlib-buffer-bytes`
- `--writer-buffer-bytes`
- `--in-flight`

**新增的参数**：
- `--profile` - 性能预设（default/lowMemory/highThroughput）

## 影响范围

- 命名空间 `fq::processing`
- 命名空间 `fq::statistic`
- CLI 命令 `stat` 和 `filter`

## 验证方式

```bash
# 单元测试
./scripts/core/test --unit

# 集成测试
./scripts/core/test --integration

# 端到端测试
./build/clang-debug/FastQTools stat -i tests/data/sample.fastq -o /tmp/stats.txt --profile lowMemory
./build/clang-debug/FastQTools filter -i tests/data/sample.fastq -o /tmp/filtered.fastq --min-quality 20 --profile highThroughput
```

## 相关规范

- 更新 `openspec/baseline/api/core-api.md`（待办）
