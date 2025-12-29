# 2025-12-26 修复 CMake package 导出与安装（find_package(FastQTools) 可用）

## 背景
- `cmake/FastQToolsConfig.cmake.in` 会 `include(FastQToolsTargets.cmake)`，但此前未正确生成/安装导出的 targets 文件。
- 在引入 `install(EXPORT ...)` 后，多个库目标的 `target_include_directories(...)` 使用了源码绝对路径，触发 CMake 导出校验错误（要求使用 build/install interface 分离）。

## 变更
- 安装与导出：
  - 安装 `include/` 公共头文件（`install(DIRECTORY include/ ...)`）。
  - 导出并安装 `FastQToolsTargets.cmake`（`install(EXPORT FastQToolsTargets ...)`）。
  - 将 `fq_core`（INTERFACE 目标）从带 `ARCHIVE/LIBRARY DESTINATION` 的安装列表中拆分，避免安装参数不匹配。
  - 不导出内部实现聚合目标（如 `fq_cli`、`fq_lib`）。
- 目标 include 路径：
  - 对导出的库目标使用 `$<BUILD_INTERFACE:...>` / `$<INSTALL_INTERFACE:...>` 设置公共 include 目录。
- Package config：
  - 在 `FastQToolsConfig.cmake.in` 中补齐 `find_dependency(...)`，确保消费方能解析第三方依赖。

## 验证
- `./scripts/build.sh clang Release`
- `./scripts/test.sh`
- `./tests/e2e/test_cli.sh`

全部通过。
