# scripts 目录检查与修复

**日期**: 2026-03-08
**类型**: fix(scripts), chore

## 变更内容

### 修复构建目录路径格式（核心问题）
- `lib/common.sh`: `get_build_dir` 输出从 `build-{compiler}-{type}` 修正为 `build/{compiler}-{type}`，与 CMakePresets.json `binaryDir: ${sourceDir}/build/${presetName}` 一致
- 影响所有调用 `get_build_dir` 的脚本：`core/build`、`core/test`、`core/lint`、`tools/coverage-report` 等

### 修复硬编码的 build- 路径
- `core/lint`: IWYU 和 clang-tidy 的构建目录搜索 glob `build-clang-*` → `build/clang-*`
- `core/lint`: 帮助文本示例路径修正
- `core/README.md`: 示例路径修正
- `tools/benchmark`: 默认 BUILD_DIR 修正
- `tools/valgrind-memcheck`: 搜索路径修正
- `tools/valgrind-cachegrind`: 搜索路径修正
- `tools/coverage-report`: 搜索路径和帮助文本修正
- `tools/package-release`: 搜索路径修正 + 去除重复项

### 移除未使用的 bz2/lzma 依赖
- `core/install-deps`: 运行时依赖移除 `libbz2-1.0`/`liblzma5`，开发依赖移除 `libbz2-dev`/`liblzma-dev`
- `ci/release-build.sh`: Alpine 移除 `bzip2-dev`/`xz-dev`，Debian 移除 `libbz2-dev`/`liblzma-dev`

### 更新 LLVM 版本
- `lib/gcov-wrapper`: `llvm-cov-19` → `llvm-cov-21`，与项目 Clang 21 工具链一致
