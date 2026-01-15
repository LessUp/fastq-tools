# devcontainer 安装 Claude Code 与 Codex CLI（2026-01-15）

## 变更

- `docker/Dockerfile.dev`
  - 增加 `nodejs`/`npm` 依赖以支持 Codex CLI 安装。
  - 在开发镜像中安装 Claude Code 与 Codex CLI，并为 `claude` 创建全局软链接。

## 影响

- 开发镜像构建时间会略有增加。
- 仅影响 devcontainer 镜像，不影响生产镜像。

## 回退方案

- 移除 `nodejs`/`npm` 安装与 CLI 安装步骤。
