# 2026-03-08 项目目录全面清理

> 本文件合并自 7 个同日同主题的独立 changelog 条目。

## 背景

项目各目录（cmake、config、docker、include、scripts、根目录）存在遗留模块、过时引用、冗余依赖，需要统一清理。

## 变更

### 1. cmake 目录

- 删除 4 个未被引用的遗留模块：`common.cmake`、`fq_common.cmake`、`fq_config.cmake`、`fq_error.cmake`
- `FastQToolsConfig.cmake.in`：移除 BZip2/LibLZMA，添加 nlohmann_json/libdeflate
- `fq_coverage.cmake`：Coverage flags 改为 CMake 列表格式

### 2. config 目录

- `profile-clang`：编译器版本 20→21
- `conanfile.py`：`exports_sources` 修正为实际存在的路径

### 3. docker 目录

- `Dockerfile.prod`/`Dockerfile.deploy`/`Dockerfile.dev`：移除未使用的 `libbz2-dev`/`liblzma-dev`
- `Dockerfile.prod`：移除冗余的 runtime-libs 收集步骤（已改为静态链接）

### 4. include/fqtools 目录

- 删除空壳 `cli/app_info.h`
- `fq.h` Façade：移除未使用的 `core/core.h`，添加 `statistics/statistic_calculator.h`

### 5. scripts 目录

- `lib/common.sh`：`get_build_dir` 输出从 `build-{compiler}-{type}` 修正为 `build/{compiler}-{type}`
- 多个脚本修复硬编码的 `build-` 路径
- 移除未使用的 bz2/lzma 依赖
- `lib/gcov-wrapper`：`llvm-cov-19` → `llvm-cov-21`

### 6. 根目录文件

- `README.md`：脚本路径、构建产物路径、依赖清单、编译器版本更新
- `CONTRIBUTING.md`：脚本路径更新
- `Doxyfile`：INPUT 添加 `include` 目录
- `conanfile.py`：`package_info.libs` 修正为实际静态库目标

### 7. changelog 目录优化

- 修复 4 个文件的命名不一致（大写→小写）
- 合并 40 个同日同主题碎片文件为 8 个
- 新增 README.md 定义命名规范和内容格式

## 影响

- 构建路径统一为 `build/{preset-name}` 格式
- 移除 bzip2/xz 未使用依赖，Conan 配方更精简
- 文档引用全部指向当前实际路径
