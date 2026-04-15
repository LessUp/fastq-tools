# 代码质量修复

## 概述

本次修复解决了项目代码审查中发现的多项问题，包括未使用的成员变量、缺失的 `[[nodiscard]]` 属性以及日志一致性问题。

## 修复详情

### 1. 移除未使用的成员变量

- **文件**: `include/fqtools/io/fastq_io.h`
- **问题**: `FastqBatch::remainderOffset_` 成员变量被声明但从未使用
- **修复**: 移除该成员变量及其在 `clear()` 方法中的初始化

### 2. 添加 `[[nodiscard]]` 属性

- **文件**: `include/fqtools/processing/predicates/min_quality_predicate.h`
- **问题**: `getName()`, `getDescription()`, `getStatistics()` 方法缺少 `[[nodiscard]]` 属性
- **修复**: 为所有 predicate 类的 getter 方法添加 `[[nodiscard]]` 属性

- **文件**: `include/fqtools/processing/mutators/quality_trimmer.h`
- **问题**: `getName()`, `getDescription()` 方法缺少 `[[nodiscard]]` 属性
- **修复**: 为所有 mutator 类的 getter 方法添加 `[[nodiscard]]` 属性

- **文件**: `include/fqtools/processing/processing_pipeline_interface.h`
- **问题**: `getPassRate()`, `getFilterRate()`, `toString()` 方法缺少 `[[nodiscard]]` 属性
- **修复**: 为 `ProcessingStatistics` 结构体的所有 getter 方法添加 `[[nodiscard]]` 属性

### 3. 日志一致性改进

- **文件**: `src/cli/commands/stat_command.cpp`
- **问题**: 错误消息使用 `std::cerr` 而非统一的日志框架
- **修复**: 改用 `fq::logging::error()` 输出错误消息

- **文件**: `src/cli/commands/filter_command.cpp`
- **问题**: 错误消息使用 `std::cerr` 而非统一的日志框架
- **修复**: 改用 `fq::logging::error()` 输出错误消息，同时保留 `spdlog` 头文件以支持日志级别检查

## 影响范围

- 公共 API 头文件（仅添加属性，不影响 ABI）
- CLI 命令实现文件
- 无破坏性变更

## 测试验证

- 所有单元测试通过
- 所有集成测试通过
- 所有端到端测试通过
- 代码格式检查通过

## 相关 Issue

- 代码审查发现的问题修复
