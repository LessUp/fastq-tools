# 2025-12-26 公共头文件迁移至 include/（core/config/error）

## 背景
- `fq_error` / `fq_config` / `fq_core` 属于可导出与可安装的公共库目标。
- 但其关键头文件此前仍位于 `src/modules/**`，会让公共 API 边界与安装包结构混乱，并迫使消费者在 include 路径中引入实现目录。

## 变更
- 将以下头文件迁移至公共 include 目录：
  - `src/modules/error/error.h` -> `include/fqtools/error/error.h`
  - `src/modules/config/config.h` -> `include/fqtools/config/config.h`
  - `src/modules/core/core.h` -> `include/fqtools/core/core.h`
- 将实现与测试中的相对 include 改为公共头 include：
  - 例如 `#include "error.h"` / `#include "config.h"` -> `#include "fqtools/error/error.h"` / `#include "fqtools/config/config.h"`
- 更新模块 CMake：
  - `src/modules/error/CMakeLists.txt` 与 `src/modules/config/CMakeLists.txt` 的源文件列表指向迁移后的头文件路径。
  - 移除对 `src/modules/**` 的私有 include 依赖，确保公共库的 include 边界仅为 `${CMAKE_INSTALL_INCLUDEDIR}`。

## 验证
- `./scripts/build.sh clang Release`
- `./scripts/test.sh`
- `./tests/e2e/test_cli.sh`

全部通过。
