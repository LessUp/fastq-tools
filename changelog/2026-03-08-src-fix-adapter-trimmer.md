# src 目录检查与修复

**日期**: 2026-03-08
**类型**: fix(processing)

## 变更内容

### 修复 AdapterTrimmer::findAdapter 3' overlap 比较 bug
- `src/processing/mutators/quality_trimmer.cpp`: `countMismatches` 调用传入完整 `sequence` 而非 `sequence.substr(i)`
- 导致 3' overlap 检测时比较了错误的序列区域（从头开始而非从位置 i 开始）
- 修复: `countMismatches(sequence, ...)` → `countMismatches(sequence.substr(i), ...)`
