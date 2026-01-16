# 修复 VS Code Devcontainer host gitconfig 挂载失败（2026-01-16）

## 背景

Dev Containers 启动时，容器创建失败，报错包含：

- `Are you trying to mount a directory onto a file (or vice-versa)?`

排查发现宿主机路径 `/tmp/fastqtools-host-gitconfig` 被意外创建为目录，导致 Docker 在挂载到容器内文件 `/tmp/host-gitconfig` 时出现“目录/文件类型不匹配”，从而阻断容器启动。

## 变更

- `.devcontainer/devcontainer.json`
  - 将 host gitconfig 临时文件从 `/tmp/fastqtools-host-gitconfig` 改为使用 `${localEnv:HOME}/.fastqtools-host-gitconfig`（稳定文件路径）。
  - `initializeCommand` 中增加对 `~/.fastqtools-host-gitconfig` 的存在性与类型校验，并将 `~/.gitconfig` 复制到该文件。
- `.devcontainer/devcontainer.simple.json`
  - 同步上述挂载与 `initializeCommand` 调整。

## 影响

- 避免因 `/tmp/fastqtools-host-gitconfig` 被创建为目录导致 Dev Containers 反复启动失败。

## 回退方案

- 回退本次对两份 devcontainer 配置中 `initializeCommand` 与 `mounts` 的修改。
