# 重构 examples/basic_usage 和 tests 目录

**日期**: 2025-02-24

## 变更概述

对 `examples/basic_usage/` 和 `tests/` 目录进行全面重构优化，修复过时引用、消除重复代码、补全缺失实现、修复潜在 bug。

## 变更详情

### examples/basic_usage/basic_stats.sh

- **扩展构建产物搜索路径**: 除 `build-clang-*` 外，新增 `build-gcc-release` / `build-gcc-debug` 搜索
- **修复构建脚本引用**: `../../scripts/build.sh` → `../../scripts/core/build`
- **移除不存在的脚本引用**: 删除对 `batch_processing.sh` 和 `paired_end_analysis.sh` 的引用
- **简化脚本**: 移除内嵌 Python HTML 报告生成（约 50 行），保持示例脚本简洁
- **跨平台兼容**: `zcat` → `gzip -dc`；`numfmt` 加 awk fallback（macOS 兼容）

### examples/basic_usage/README.md

- **修复构建命令路径**: `../../scripts/build.sh` → `../../scripts/core/build`

### tests/CMakeLists.txt — 消除重复

- 在 `tests/CMakeLists.txt` 中新增共享函数 `add_fq_test(name label timeout sources...)`
- `tests/unit/CMakeLists.txt`: 移除重复的 `add_unit_test` 函数（约 30 行），改用 `add_fq_test`
- `tests/integration/CMakeLists.txt`: 移除重复的 `add_integration_test` 函数（约 30 行），改用 `add_fq_test`

### tests/utils/fixture_loader.cpp — 补全缺失实现

- 实现 `FixtureLoader::createTempFastq()`: 生成指定条数和读长的临时 FASTQ 文件
- 实现 `FixtureLoader::compareFiles()`: 二进制比较两个文件内容
- 添加所需头文件: `<random>`, `<algorithm>`

### tests/utils/test_helpers.cpp — 线程安全修复

- `generateRandomDNA()`: `static std::mt19937` → `thread_local std::mt19937`
- `generateRandomQuality()`: 同上

### tests/e2e/test_advanced_cli.py — 修复文件读取

- 新增 `_is_gzip()` / `_read_fastq_lines()` / `_read_fastq_content()` 辅助方法
- 自动检测 gzip magic bytes，兼容 gzip 和普通文本两种输出格式
- 替换原有硬编码的 `gzip.open` 调用

### tests/README.md — 修复文档

- 所有 `./scripts/test` → `./scripts/core/test`
- 所有 `./scripts/build` → `./scripts/core/build`
- 目录树结构补充 `error/` 和 `memory/` 子目录

## 影响范围

- **examples/**: 示例脚本和文档
- **tests/**: CMake 构建配置、测试工具库、E2E 测试、文档
- **无功能性变更**: 不影响 src/ 下的任何生产代码
