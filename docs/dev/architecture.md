# 架构设计

## 核心设计原则

- **性能优先**: 最大化处理吞吐量和资源利用率
- **现代化 C++**: 全面使用 C++20 标准特性
- **高内聚低耦合**: 清晰的模块边界和接口设计

## 技术栈

| 领域 | 技术 | 理由 |
|------|------|------|
| **语言** | C++20 | 现代特性，性能卓越 |
| **构建** | CMake | C++ 项目标准 |
| **依赖** | Conan | 自动化第三方库管理 |
| **并发** | Intel TBB | 高性能并行计算 |
| **CLI** | cxxopts | 轻量级命令行解析 |
| **日志** | spdlog | 高性能日志库 |
| **测试** | GoogleTest | 功能强大的测试框架 |

## 架构层次

```
用户/调用者
    ↓
CLI 层 (src/cli/)
    ↓
应用命令层 (src/cli/commands/)
    ↓
核心库层 (include/fqtools/)
    ↓
实现层 (src/pipeline/, src/modules/)
```

### 1. CLI 层
- 入口: `src/cli/main.cpp`
- 职责: 参数解析、命令分发、日志初始化
- 使用: cxxopts + spdlog

### 2. 应用命令层
- 接口: `CommandInterface` (`command_interface.h`)
- 实现: `StatCommand`, `FilterCommand`
- 职责: 参数验证、业务逻辑调用

### 3. 核心库层
- 公共 API: `include/fqtools/`
- 主要接口:
  - `processing_pipeline.h`: `ProcessingPipelineInterface`
  - `statistic_calculator.h`: `StatisticCalculatorInterface`
  - `fq.h`: 聚合头文件

### 4. 实现层
- 流水线: `src/pipeline/`
- 统计: `src/pipeline/statistics/`
- 通用模块: `src/modules/`

## 并发模型

使用 `tbb::parallel_pipeline` 实现高性能并行处理：

- **source**: 串行读取 FASTQ 记录
- **processing**: 并行过滤和变换
- **sink**: 串行写入结果

内置背压机制，自动平衡 I/O 和计算速度。

## 构建系统

采用现代 CMake 目标拆分：

```cmake
# 基础模块
fq_common
fq_error
fq_config
fq_core
fq_modern_io
 
# 核心功能
fq_pipeline
 
# 应用层
fq_cli
```

依赖方向保持自下而上，确保模块化设计。
