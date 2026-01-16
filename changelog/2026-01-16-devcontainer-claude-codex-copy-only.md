# 调整 Devcontainer 的 Claude/Codex 配置同步策略（2026-01-16）

## 背景

此前将宿主机 `~/.claude` 与 `~/.codex` 目录整体挂载到容器内，可能导致配置冲突或容器内配置被宿主机覆盖，影响工具稳定性。

## 变更

- `.devcontainer/devcontainer.json`
  - 移除宿主机 `.claude` / `.codex` 目录的整体挂载。
  - 改为挂载宿主机暂存目录 `~/.fastqtools-host-claude` / `~/.fastqtools-host-codex` 到容器 `/tmp/host-claude` / `/tmp/host-codex`，再由容器脚本复制到 `/home/developer/.claude` 与 `/home/developer/.codex`。
  - 通过 `.devcontainer/scripts/host-prepare.sh` 在宿主机准备暂存目录与文件。
  - 通过 `.devcontainer/scripts/container-setup.sh` 在容器内执行配置同步。
- `.devcontainer/devcontainer.simple.json`
  - 同步上述调整。

## 影响

- 容器内 Claude/Codex 配置只读拷贝，不再与宿主机目录进行双向挂载，降低冲突风险。

## 回退方案

- 恢复 `.claude` / `.codex` 目录挂载与原有初始化逻辑。
