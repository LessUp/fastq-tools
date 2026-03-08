# tests 目录检查与修复

**日期**: 2026-03-08
**类型**: fix(tests)

## 变更内容

### 修复 e2e 测试中构建产物路径
- `e2e/test_cli.sh`: 默认 FASTQTOOLS 路径从 `build-clang-release` 修正为 `build/clang-release`
- `e2e/test_advanced_cli.py`: 默认 FASTQTOOLS 路径从 `build-clang-release` 修正为 `build/clang-release`
- 与 CMakePresets.json `binaryDir: ${sourceDir}/build/${presetName}` 保持一致
