# 构建指南

构建系统的目标是把“本地开发”“CI 验证”“性能测试”尽量统一到同一套入口下，减少每个人各自拼装命令导致的漂移。

## 推荐入口

在仓库根目录使用 `scripts/core/*`：

```bash
./scripts/core/build
./scripts/core/build --dev
./scripts/core/build --compiler gcc --type Debug
./scripts/core/build --sanitizer asan
```

这些脚本会协调 Conan、CMake preset 与默认构建目录，避免维护者手动记忆过多环境细节。

## 常见构建场景

| 场景 | 推荐命令 | 说明 |
| --- | --- | --- |
| 日常验证 | `./scripts/core/build --dev` | 生成更适合调试的构建 |
| 发布前检查 | `./scripts/core/build` | 默认 Release 构建 |
| Sanitizer 排障 | `./scripts/core/build --sanitizer asan` | 用于定位内存与未定义行为问题 |
| 覆盖率 | `./scripts/core/build --coverage` | 配合测试脚本生成覆盖率数据 |

## 目录与产物

默认构建目录遵循 `build/<preset>` 形式，例如 `build/clang-debug`、`build/clang-release`。维护脚本、CI 与文档应尽量引用这种统一约定，而不是各自假设一个单独路径。

## 什么时候需要手动下钻

只有在调试 Conan、preset 或特定 CMake 目标时，才建议直接手写 `conan install` / `cmake --preset` 命令。平时请优先使用脚本入口，因为它们同时也是仓库默认工作流的一部分。

## 构建之后要做什么

构建成功只说明“能编译”。如果改动会影响行为，还需要继续运行[`测试策略`](./testing)中对应的验证；如果改动会影响性能叙事，还要补看[`Benchmark 指南`](./benchmark-guide)。
