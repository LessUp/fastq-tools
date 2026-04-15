# 2025-12-26 外部消费方验证（find_package(FastQTools)）

## 目标
验证 FastQTools 以 CMake package 形式安装后，可在独立工程中通过 `find_package(FastQTools CONFIG)` 正常发现，并能链接导出目标完成编译与运行。

## 验证方式
- 将 FastQTools 安装到仓库内本地前缀：`_cmake_install_prefix/`。
- 新增最小 consumer 工程：`tests/cmake_package_consumer/`。
  - `find_package(FastQTools CONFIG REQUIRED)`
  - `target_link_libraries(fqtools_consumer PRIVATE fq_common)`
  - `#include "fqtools/common/common.h"` 并调用 `fq::common::trim()`
- consumer 配置时通过 `CMAKE_PREFIX_PATH` 同时指向：
  - FastQTools 安装前缀（用于 `FastQToolsConfig.cmake`/targets）
  - Conan CMakeDeps 输出目录（用于 `find_dependency(...)` 解析第三方依赖）

## 结果
- consumer 工程成功配置、编译与运行。

## 说明
- consumer 工程用于回归验证导出/安装链路，避免安装包对源码目录或私有 include 路径存在隐式依赖。
