# include/fqtools 目录检查与修复

**日期**: 2026-03-08
**类型**: refactor(api), chore

## 变更内容

### 删除空壳文件
- 删除 `cli/app_info.h` — 空文件，全项目无引用

### 修复 fq.h Façade
- 移除 `fqtools/core/core.h` 引用 — 项目源码和测试均未使用，不应在 Façade 中暴露
- 添加 `fqtools/statistics/statistic_calculator.h` — 包含工厂函数的兼容代理，使 Façade 覆盖完整

### 保留的文件（确认正常）
- `core/core.h` — 保留文件但不在 Façade 中暴露（含 QualityScore/SequenceUtils 工具类，按需引用）
- `processing/processing_pipeline.h` — 兼容代理，转发到 interface 头文件
- `statistics/statistic_calculator.h` — 兼容代理，转发到 interface 头文件
