# cmake 目录清理与修复

**日期**: 2026-03-08
**类型**: chore(build)

## 变更内容

### 删除遗留模块
- 删除 `cmake/modules/common.cmake` — 未被引用，引用不存在的 `src/modules/common/` 路径
- 删除 `cmake/modules/fq_common.cmake` — 未被引用，与 `src/common/CMakeLists.txt` 重复定义 `fq_common` 目标
- 删除 `cmake/modules/fq_config.cmake` — 未被引用，与 `src/config/CMakeLists.txt` 重复定义 `fq_config` 目标
- 删除 `cmake/modules/fq_error.cmake` — 未被引用，与 `src/error/CMakeLists.txt` 重复定义 `fq_error` 目标

### 修复 FastQToolsConfig.cmake.in
- 移除未使用的 `BZip2` 和 `LibLZMA` 依赖声明
- 添加项目实际使用的 `nlohmann_json` 和 `libdeflate` 依赖声明

### 修复 fq_coverage.cmake
- Coverage flags 从字符串格式改为 CMake 列表格式，与 `fq_sanitizers.cmake` 保持一致
