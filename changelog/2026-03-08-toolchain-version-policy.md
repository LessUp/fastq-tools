# 工具链版本规范制定

**日期**: 2026-03-08
**类型**: docs
**范围**: build, docker, ci

## 变更内容

- 新建 `docs/decisions/toolchain-policy.md`，定义项目编译器、Docker 镜像、构建工具的版本选型规范与升级策略
- 明确开发环境（GCC 15 / Clang 21）与社区/生产环境（GCC 14 / Clang 19）的双轨版本策略
- 将 CMake 最低版本要求从 3.20 提升到 3.28（Ubuntu 24.04 LTS 自带版本）
- 运行时 Docker 镜像确定为 `debian:bookworm-slim`
- 社区构建镜像确定为 `gcc:14.x-bookworm`

## 影响文件

- `docs/decisions/toolchain-policy.md`（新建）
- `AGENTS.md`（更新编译器版本、Docker 镜像、工具链规范引用）
- `CMakeLists.txt`（`cmake_minimum_required` 3.20 → 3.28）
- `CMakePresets.json`（`cmakeMinimumRequired` 3.20 → 3.28）
- `docs/dev/build.md`（更新系统要求）
- `docs/README.md`（添加工具链规范文档索引）
