# 2025-12-25 核心模块头文件 include 边界修正

## 背景
- `src/modules/core/core.h` 仍通过 `#include "../error/error.h"` 这种相对路径引用内部实现目录。
- 相对 include 破坏了“只通过 `include/fqtools/**` 暴露公共 API”的约束，也会在后续目录重排时造成脆弱依赖。

## 变更
- 将 `core.h` 中的 `#include "../error/error.h"` 改为 `#include "fqtools/error/error.h"`，统一走公共头文件入口。

## 验证
- `./scripts/build.sh clang Release`
- `./scripts/test.sh`
- `./tests/e2e/test_cli.sh`

全部通过。
