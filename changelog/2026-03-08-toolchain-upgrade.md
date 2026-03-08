# 2026-03-08 工具链版本规范制定与升级

> 本文件合并自 3 个同日同主题的独立 changelog 条目。

## 背景

项目需要明确工具链版本选型规范，并将 Clang 从 21 升级到 22、C++ 标准从 C++20 升级到 C++23。

## 变更

### 1. 工具链版本规范制定

- 新建 `docs/decisions/toolchain-policy.md`：定义编译器、Docker 镜像、构建工具的版本选型与升级策略
- CMake 最低版本从 3.20 提升到 3.28（Ubuntu 24.04 LTS 自带版本）
- 运行时 Docker 镜像确定为 `debian:bookworm-slim`

### 2. 统一工具链版本（开发与生产一致）

- 从"开发/社区分离"策略改为"全环境统一"策略
- GCC 15.x、Clang 21、Docker 构建镜像 `gcc:15.x-bookworm` 统一用于开发、CI、生产
- CI 矩阵：GCC 15 + Clang 21 为主力，GCC 14 + Clang 19 为兼容性检查（allow-failure）

### 3. Clang 21→22、C++20→C++23 升级

| 组件 | 旧版本 | 新版本 |
|------|--------|--------|
| Clang/LLVM | 21 | 22 |
| C++ 标准 | C++20 | C++23 |
| GCC | 15.2 | 15.2（不变） |

受影响文件：
- 构建系统：`CMakeLists.txt`、`CMakePresets.json`、`config/conan/profile-clang`、`config/conan/profile-gcc`
- Docker：`docker/Dockerfile.dev`
- CI/CD：`.github/workflows/ci.yml`、`quality.yml`
- 脚本：`scripts/ci/release-build.sh`、`scripts/tools/install-llvm`、`scripts/core/lint`、`scripts/lib/gcov-wrapper`
- 文档：`AGENTS.md`、`CLAUDE.md`、`README.md`、`CONTRIBUTING.md`、`docs/decisions/toolchain-policy.md` 等

## 影响

- 所有环境统一使用 GCC 15.2 + Clang 22 + C++23
- 与 fq-compressor 项目保持工具链一致
