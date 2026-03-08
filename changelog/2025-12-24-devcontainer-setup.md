# 2025-12-24 DevContainer 配置与仓库元文件补齐

> 本文件合并自 2 个同日同主题的独立 changelog 条目。

## 背景

需要为 VS Code Dev Containers 提供可用配置，同时补齐 Git 文本属性、提交模板和 Release 自动化。

## 变更

### 1. VS Code DevContainer 配置

- 新增 `.devcontainer/devcontainer.json`：使用 `docker-compose.yml` 的 dev 服务
- 指定 `remoteUser=developer`，`postCreateCommand` 执行 `conan profile detect`
- 配置推荐扩展（CMake Tools、clangd 等）
- `docker/Dockerfile.dev` 补齐 clangd-19 / clang-tidy-19 / clang-format-19

### 2. 仓库元文件与 Release 自动化

- `docker/docker-compose.yml`：挂载路径 `~` → `${HOME}` 避免不展开问题
- `docker/Dockerfile.dev` / `docker/Dockerfile`：Conan 版本对齐 CI（2.19.0），工具链对齐 Clang 19
- 新增 `.gitattributes`：统一 `eol=lf`，标记二进制文件
- 新增 `.gitmessage.txt`：Conventional Commits 提交模板
- 新增 `.vscode/settings.json`：启用 `git.useEditorAsCommitInput`
- 新增 `.github/workflows/release.yml`：基于 tag `v*` 触发发布

## 影响

- 仅影响开发环境与发布流程配置，不改变核心业务逻辑
