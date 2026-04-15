# Docker 镜像补充常用开发工具（2026-01-17）

## 背景

开发与构建镜像缺少 vim 等常用命令行工具，影响容器内临时排查与编辑。

## 变更

- `docker/Dockerfile`
  - 在构建阶段安装 vim/neovim/less/tree/grep/htop/tmux/ripgrep/zip/unzip/jq 等常用工具。
- `docker/Dockerfile.dev`
  - 在基础工具列表中加入 vim/neovim/less/tree/grep/htop/tmux/ripgrep/zip/unzip/jq。

## 影响

- 容器内具备基础编辑与排查工具，便于临时调试。

## 回退方案

- 移除 Dockerfile 与 Dockerfile.dev 中新增的工具包。
