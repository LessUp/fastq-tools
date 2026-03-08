# 2025-10-21 架构重构（Phase 1-2 + 公共 API 收口）

> 本文件合并自 4 个同日同主题的独立 changelog 条目。

## 背景

通过多阶段重构，建立公共 API 聚合头、引入 `fq_pipeline` 模块、收口对外头文件，降低 CLI 对实现细节的耦合。

## 变更

### Phase 1：公共聚合头与 CLI 解耦

- 新增 `include/fqtools/fq.h`：导出稳定 API（接口 + 工厂 + 兼容封装）
- CLI `main.cpp` 从 `core_legacy/core.h` 改为 `#include <fqtools/fq.h>`
- 异常宏 `FQ_THROW_VALIDATION_ERROR` 支持 2/3 参调用
- CMake 为 `fq_lib` 增加 INTERFACE 头文件目录

### Phase 2：fq_pipeline 模块落地

- 新增 `src/modules/pipeline/` 模块（STATIC 库），聚合 processing/statistics/memory/factory
- `fq_lib` 显式聚合 `fq_pipeline`、`fq_common`、`fq_error`、`fq_core`、`fq_io`、`fq_fastq`
- 从 `fq_legacy_lib` 移除与 pipeline 重复编译的源

### Phase 2：公共 API 头收口

- 新增 `include/fqtools/processing_pipeline.h`、`statistic_calculator.h` 转发头
- CLI 命令改用公共头路径
- 新增 pipeline 冒烟测试
- `docs/dev/architecture.md` 增补公共 API 头目录章节

### 补充：架构图

- 绘制项目模块与依赖架构图，提供可视化参考

## 影响

- 外部包含路径 `<fqtools/processing_pipeline.h>`、`<fqtools/statistic_calculator.h>` 稳定可用
- 旧包含路径仍兼容
- 为后续模块细化与 CMake 目标拆分打基础
