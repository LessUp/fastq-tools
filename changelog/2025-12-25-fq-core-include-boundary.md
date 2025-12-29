# 2025-12-25 收敛 fq_core 头文件暴露范围

## 背景
- `fq_core`（INTERFACE 目标）通过 `target_include_directories` 将 `src/modules/core/` 暴露给所有依赖方。
- 这与“仅通过 `include/fqtools/**` 提供公共 API”的结构化约束冲突，也让实现细节泄露给外部。

## 变更
- 将 `src/modules/core/CMakeLists.txt` 中的 `target_include_directories(fq_core ...)` 调整为只公开 `${CMAKE_SOURCE_DIR}/include`，杜绝实现路径外泄。

## 验证
- `./scripts/build.sh clang Release`
- `./scripts/test.sh`
- `./tests/e2e/test_cli.sh`

全部通过。
