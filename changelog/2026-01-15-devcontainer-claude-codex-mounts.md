# devcontainer 挂载 Claude/Codex 配置目录（2026-01-15）

## 变更

- `.devcontainer/devcontainer.json`
  - 新增可写挂载：`${HOME}/.claude`、`${HOME}/.codex` 到 `/home/developer`。
  - 初始化时确保宿主侧目录存在，避免挂载失败。
- `.devcontainer/devcontainer.simple.json`
  - 新增可写挂载：`${HOME}/.claude`、`${HOME}/.codex` 到 `/home/developer`。
  - 初始化时确保宿主侧目录存在，避免挂载失败。

## 影响

- 容器内可复用宿主侧 Claude/Codex 的配置目录，并保持可写同步。
- 仅影响 devcontainer 环境，不影响生产镜像。

## 回退方案

- 移除对应 devcontainer 的 `.claude` / `.codex` 挂载与初始化目录创建逻辑。
