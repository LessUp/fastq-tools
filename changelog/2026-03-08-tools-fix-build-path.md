# tools 目录检查与修复

**日期**: 2026-03-08
**类型**: fix(tools)

## 变更内容

### 修复构建产物路径
- `benchmark/scripts/run_baseline.sh`: 默认 FASTQTOOLS 路径从 `build-clang-release` 修正为 `build/clang-release`
- 与 CMakePresets.json `binaryDir: ${sourceDir}/build/${presetName}` 保持一致
