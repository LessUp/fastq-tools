# 代码审查与优化：include/fqtools、src、scripts

**日期**: 2025-03-07
**类型**: fix / refactor / style
**范围**: common, cli, processing, statistics, benchmark, io

## 修复

### 严重 Bug：IDGenerator::reset() 无法正确重置计数器
- **文件**: `src/common/common.cpp`
- **问题**: `nextId()` 和 `reset()` 各自定义了独立的 `static std::atomic` 变量，
  导致 `reset()` 操作的是一个与 `nextId()` 无关的计数器，无法真正重置 ID 生成器
- **修复**: 将 static 计数器提取为文件级匿名命名空间变量，确保两个函数操作同一个计数器

### processBatch 中 modifiedReads 统计缺失
- **文件**: `src/processing/processing_pipeline.cpp`
- **问题**: `ProcessingStatistics::modifiedReads` 字段从未被更新，
  `toString()` 输出始终显示 0
- **修复**: 在 `processBatch` 中记录 mutator 修改前后的长度变化并累加统计，
  同时在 TBB 管道聚合阶段补充 `modifiedReads` 传递

## 重构

### 清理 cli/app_info.h 无用依赖
- **文件**: `include/fqtools/cli/app_info.h`
- **问题**: 包含了 `processing_pipeline_interface.h` 和 `statistic_calculator_interface.h`
  两个完全无关的头文件，且定义了一个空函数体 `softwareInfo()`
- **修复**: 移除无关依赖和空函数，保留为预留扩展的最小头文件

### 清理 statistic_interface.h 不必要的包含
- **文件**: `include/fqtools/statistics/statistic_interface.h`
- **问题**: 不必要地包含 `fqtools/cli/app_info.h`（历史遗留注释 "For WithID replacement"）
- **修复**: 移除不必要的包含

### 优化 fq.h Façade 头文件
- **文件**: `include/fqtools/fq.h`
- **改进**:
  - 使用聚合头文件 `mutators.h` / `predicates.h` 替代逐个包含具体实现
  - 补充缺失的模块：`error/error.h`、`config/config.h`、`core/core.h`、
    `io/fastq_io.h`、`io/fastq_reader.h`、`io/fastq_writer.h`
  - 添加分组注释提高可读性

### 统一日志包含方式
- **文件**: `src/statistics/fq_statistic.cpp`
- **问题**: 直接包含 `spdlog/spdlog.h` 而非项目统一的 `fqtools/logging.h` 包装
- **修复**: 移除多余的 `spdlog/spdlog.h` 包含（已通过 `fqtools/logging.h` 间接包含）

## 风格修正

### 消除 std::endl 使用
- **文件**: `src/cli/commands/stat_command.cpp`、`src/cli/commands/filter_command.cpp`
- **问题**: 多处使用 `std::endl`，违反编码规范（项目要求使用 `'\n'`）
- **修复**: 全部替换为 `'\n'`

## 兼容性

### data_collector.cpp 跨平台兼容
- **文件**: `src/benchmark/data_collector.cpp`
- **问题**: `gmtime_r` 是 POSIX 特有函数，缺少 Windows 兼容处理
  （与 `common.cpp` 中 `localtime_r` 的处理不一致）
- **修复**: 添加 `#ifdef _WIN32` 分支使用 `gmtime_s`

## scripts 审查结论

- `scripts/core/build`、`scripts/core/test`、`scripts/core/lint` 质量良好
- `scripts/lib/common.sh` 公共函数库设计合理，导出机制完整
- 未发现需要修改的问题
