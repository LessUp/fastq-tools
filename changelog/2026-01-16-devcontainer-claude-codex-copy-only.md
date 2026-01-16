# 调整 Devcontainer 的 Claude/Codex 配置同步策略（2026-01-16）

## 背景

此前将宿主机 `~/.claude` 与 `~/.codex` 目录整体挂载到容器内，可能导致配置冲突或容器内配置被宿主机覆盖，影响工具稳定性。

## 变更

- `.devcontainer/devcontainer.json`
  - 移除宿主机 `.claude` / `.codex` 目录的整体挂载。
  - 改为仅挂载指定文件到 `/tmp/host-claude` / `/tmp/host-codex`，并在容器内复制到 `/home/developer/.claude` 与 `/home/developer/.codex`。
  - 新增宿主机侧临时目录 `~/.fastqtools-host-claude` / `~/.fastqtools-host-codex`，用于承载需要同步的单文件。
- `.devcontainer/devcontainer.simple.json`
  - 同步上述调整。

## 影响

- 容器内 Claude/Codex 配置只读拷贝，不再与宿主机目录进行双向挂载，降低冲突风险。

## 回退方案

- 恢复 `.claude` / `.codex` 目录挂载与原有初始化逻辑。
