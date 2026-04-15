# clang 19 全链路对齐（2025-12-24）

## 背景
- 仓库 CI 已使用 LLVM/Clang 19，但本地 Docker/DevContainer 仍存在 Clang 17 等残留，容易导致行为不一致（编译/静态检查/覆盖率工具链不同）。

## 本次变更
- Docker/Dev Container
  - 更新 `docker/Dockerfile.dev`
    - 使用 `llvm.sh 19` 安装 LLVM/Clang 19 工具链（含 `clang`/`clang++`/`clangd`/`clang-tidy`/`clang-format`/`llvm`）。
    - 通过 `update-alternatives` 统一默认命令名，避免脚本/工具依赖具体版本路径。
    - `CC/CXX` 环境变量改为 `clang/clang++`。
  - 更新 `docker/Dockerfile`
    - 构建阶段工具链对齐到 LLVM/Clang 19，并设置 `update-alternatives`。

- CI/Release
  - 更新 `.github/workflows/ci.yml`
    - Coverage Job 补齐 clang/llvm 安装与 `update-alternatives`，保证 `./scripts/test.sh -c clang -t Coverage` 可用。

- 文档
  - 更新 `docs/dev/build.md` 与 `README.md`
    - 推荐工具链口径对齐到 Clang 19（与 CI/Dev Container 一致）。
  - 更新 `docs/user/usage.md` 与 `README.md`
    - 修复构建目录示例大小写不一致（`build-clang-release`）。

## 影响范围
- 仅影响工具链与开发/CI 配置，不改变核心功能逻辑。
