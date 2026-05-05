# 重构：架构改进与代码质量提升

## 目标

根据架构分析结果，实施 6 项重构任务，提升代码的模块深度、可测试性和可维护性。

## 主要改动

### 1. 添加 I/O 接口层 (IReader/IWriter)

**问题**: `FastqReader` 和 `FastqWriter` 是具体类，没有接口层。Pipeline 直接实例化它们，导致测试必须使用真实文件系统。

**解决方案**:
- 新增 `IReader` 和 `IWriter` 抽象接口（`include/fqtools/io/reader_interface.h`, `writer_interface.h`）
- `FastqReader` 和 `FastqWriter` 实现这些接口
- `ProcessingPipelineInterface` 新增 `setReader()` 和 `setWriter()` 方法支持依赖注入
- 注入自定义 Reader/Writer 时自动使用串行模式

**收益**: 测试可注入 mock reader/writer，无需文件系统 I/O。

### 2. 提取 resolveMaxInFlightBatches 到共享工具

**问题**: 同一个函数在 `processing_pipeline.cpp` 和 `fq_statistic.cpp` 中完全重复。

**解决方案**:
- 提取到 `fq::common::resolveMaxInFlightBatches()`（`include/fqtools/common/common.h`）
- 两处调用改为使用共享函数

**收益**: 知识集中在一处，修改一次两处生效。

### 3. 提取 StatisticsWriter 类

**问题**: `writeResult()` 方法包含复杂的格式化逻辑，测试需要运行整个统计管道。

**解决方案**:
- 新增 `StatisticsWriter` 类（`include/fqtools/statistics/statistics_writer.h`）
- 接受 `std::ostream&` 参数，支持输出到任意流
- 格式化逻辑从 `FastqStatisticCalculator` 中分离

**收益**: 格式化逻辑可独立测试，测试更简单快速。

### 4. 删除浅层包装头文件

**问题**: `processing_pipeline.h` 和 `statistic_calculator.h` 只是单行 include，无任何抽象价值。

**解决方案**:
- 删除这两个文件
- 更新所有引用直接指向 `*_interface.h`

**收益**: 减少认知负担，代码结构更清晰。

### 5. 统一配置结构

**问题**: CLI 层的 `FilterCommand::Config` 包含大量未使用的冗余字段。

**解决方案**:
- 简化 `FilterCommand::Config` 只保留输入输出路径
- 其他配置直接使用 `fq::processing::ProcessingConfig`

**收益**: 配置定义更集中，避免默认值分歧。

### 6. ErrorHandler 系统

**决定**: 保留但标注为可选功能。虽然有测试覆盖但未被生产代码使用，但不产生负面影响，未来可能有用。

## 影响范围

- 新增文件: 4 个（2 个接口头文件，1 个 StatisticsWriter 头文件，1 个实现文件）
- 删除文件: 2 个（浅层包装头文件）
- 修改文件: 约 15 个

## 验证

- 所有 13 个测试通过
- 代码格式化完成
- 构建成功（无错误）

## 相关文档

- [AGENTS.md](AGENTS.md) — AI Agent 规则
- [openspec/baseline/architecture/0001-core-architecture.md](openspec/baseline/architecture/0001-core-architecture.md) — 核心架构
