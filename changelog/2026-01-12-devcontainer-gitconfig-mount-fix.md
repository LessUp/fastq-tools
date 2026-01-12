# 修复 VS Code Devcontainer ~/.gitconfig 挂载失败（2026-01-12）

## 背景

在 VS Code 执行 “Reopen in Container” 时，`mounts` 中的 `${localEnv:HOME}/.gitconfig` 在部分环境下会被展开为 `/.gitconfig`（`HOME` 为空或异常），导致 Docker 将其创建为目录并尝试以目录形式挂载到容器内文件 `/tmp/host-gitconfig`，从而报错：

- `Are you trying to mount a directory onto a file (or vice-versa)?`

## 变更

- `.devcontainer/devcontainer.json`
  - 新增 `initializeCommand`：在宿主机侧确保 `~/.gitconfig` 存在且为文件（若为目录则报错并提示修复）。
  - 调整 `mounts`：不再直接挂载 `~/.gitconfig`，改为将宿主机 `~/.gitconfig` 复制到 `/tmp/fastqtools-host-gitconfig` 并从该路径进行 bind mount，避免 `${localEnv:HOME}` 展开异常导致路径变为 `/.gitconfig`。
- `.devcontainer/devcontainer.simple.json`
  - 同步上述 `initializeCommand` 与 `mounts` 调整。

## 影响

- 修复 VS Code Dev Containers 在特定环境下因 `HOME` 变量异常导致的容器启动失败。
- 现有容器内 gitconfig 同步逻辑保持不变。

## 回退方案

- 回退这两份 devcontainer 配置中的 `initializeCommand` 与 `mounts` 修改。
