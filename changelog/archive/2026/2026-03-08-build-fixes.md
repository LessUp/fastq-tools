# 2026-03-08 构建路径与代码修复

> 本文件合并自 7 个同日同主题的独立 changelog 条目。

## 背景

多个目录中的构建产物路径仍使用旧格式 `build-{compiler}-{type}`，需统一为 `build/{compiler}-{type}`。同时修复若干代码 Bug。

## 变更

### 1. 构建产物路径修正

- `examples/basic_usage/`：4 个示例脚本搜索路径修正为 `build/{compiler}-{type}`
- `tests/e2e/test_cli.sh`、`test_advanced_cli.py`：默认路径修正
- `tools/benchmark/scripts/run_baseline.sh`：默认路径修正

### 2. CI 修复：Clang + libc++ 链接

- `config/conan/profile-clang`：添加 `-fuse-ld=lld` 链接器标志
- `config/dependencies/conanfile.py`：Clang + libc++ 场景追加 linker flags
- `.github/workflows/ci.yml`：安装 lld-21 并设置 `update-alternatives`

### 3. Docker 部署镜像运行时修复

- `docker/Dockerfile.deploy`：添加 `-static-libstdc++ -static-libgcc`，与 `Dockerfile.prod` 策略一致
- 移除运行时阶段对 `libstdc++6` 的依赖

### 4. 代码修复

- `src/processing/mutators/quality_trimmer.cpp`：修复 `AdapterTrimmer::findAdapter` 3' overlap 比较 Bug — `countMismatches(sequence, ...)` → `countMismatches(sequence.substr(i), ...)`
- `src/common/common.cpp`：合并 `fmt::format` 不必要换行，通过 clang-format 检查

## 影响

- 所有构建产物路径与 `CMakePresets.json` 的 `binaryDir` 保持一致
- CI Clang + libc++ 构建和链接稳定
- AdapterTrimmer 3' 端适配器检测结果正确
