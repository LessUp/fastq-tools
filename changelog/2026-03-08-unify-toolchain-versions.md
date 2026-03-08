# 统一工具链版本：开发与生产环境一致

**日期**: 2026-03-08
**类型**: docs
**范围**: build, docker, ci

## 变更内容

将编译器和 Docker 镜像从"开发/社区分离"策略改为"全环境统一"策略：

- **GCC**：开发、CI、生产统一使用 GCC 15.x（原方案：开发 15 / 社区 14）
- **Clang**：开发、CI、Sanitizer 统一使用 Clang 21（原方案：开发 21 / 社区 19）
- **Docker 构建镜像**：统一使用 `gcc:15.x-bookworm`（原方案：社区用 `gcc:14.x-bookworm`）
- **CI 矩阵**：GCC 15 + Clang 21 为主力（必须通过），GCC 14 + Clang 19 为兼容性检查（allow-failure）

## 决策理由

1. 项目通过 Docker 分发预编译二进制，用户无需自行编译，编译器版本对用户透明
2. 统一版本消除"开发能编译、生产编译失败"的环境差异问题
3. 维护一套配置比维护两套的认知负担和出错概率显著更低
4. Dockerfile.prod 和 Dockerfile.deploy 实际已在使用 GCC 15.2，统一是让规范与现实一致

## 影响文件

- `docs/decisions/toolchain-policy.md`（全面重写为统一方案）
- `AGENTS.md`（版本表、Docker 镜像、选型原则同步更新）
- `CLAUDE.md`（与 AGENTS.md 同步）
- `docs/dev/build.md`（编译器版本描述更新）
