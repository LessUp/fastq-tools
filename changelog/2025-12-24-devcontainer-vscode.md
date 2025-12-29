# 增加 VS Code Dev Container 配置（2025-12-24）

## 背景
- 为 FastQTools 增加可用于 VS Code Remote - Containers / Dev Containers 的开发容器配置。
- 复用仓库已有 `docker/Dockerfile.dev` 与 `docker/docker-compose.yml`，保持 KISS。

## 本次变更
- 新增 `/.devcontainer/devcontainer.json`
  - 使用 `docker/docker-compose.yml` 的 `dev` 服务作为开发容器。
  - 指定 `remoteUser=developer`，工作目录为 `/workspace`。
  - `postCreateCommand` 进行 `conan profile detect`，避免首次构建时缺少 profile。
  - 配置推荐扩展（CMake Tools、clangd 等）与基础设置。
- 更新 `docker/Dockerfile.dev`
  - 补齐 `clangd-19` / `clang-tidy-19` / `clang-format-19`（通过 LLVM 19 工具链安装），确保 VS Code 语言服务与静态分析开箱即用。

## 影响范围
- 仅影响开发环境容器配置，不改变产物与运行逻辑。

## 使用方式
- VS Code 打开仓库后选择：Dev Containers: Reopen in Container。
- 容器内构建可直接复用项目脚本：
  - `./scripts/build.sh`
  - `./scripts/test.sh`
