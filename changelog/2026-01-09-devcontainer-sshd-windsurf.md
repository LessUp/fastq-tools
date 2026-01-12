# Devcontainer 增加 sshd 以兼容 Windsurf/Cursor（2026-01-09）

## 变更

- `docker/Dockerfile.dev`
  - 安装 `openssh-server`、`openssh-client`、`procps`，用于在开发容器内启动 sshd。
- `docker/docker-compose.yml`
  - 为 `dev` 服务新增 SSH 端口映射：默认 `127.0.0.1:${FASTQTOOLS_SSH_PORT:-2222} -> 2222`。
  - 支持通过 `FASTQTOOLS_SSH_BIND`/`FASTQTOOLS_SSH_PORT` 配置绑定地址与端口。
- `.devcontainer/setup-sshd.sh`
  - 新增：幂等配置 sshd（端口 2222、禁密码、禁 root、仅允许 `developer`）。
  - 新增：处理只读挂载 `~/.ssh` 的场景，提供可写的 `authorized_keys` fallback。
- `.devcontainer/start-sshd.sh`
  - 新增：容器启动时检测并拉起 sshd。
- `.devcontainer/devcontainer.json` / `.devcontainer/devcontainer.simple.json`
  - 在 `postCreateCommand`/`postStartCommand`/`postAttachCommand` 末尾追加 sshd 配置与启动逻辑。
- `docs/dev/windsurf-cursor-devcontainer-ssh.md`
  - 新增：方案说明与 Windsurf/Cursor 连接步骤。

## 影响

- VS Code Dev Containers 使用方式保持不变。
- Windsurf/Cursor 可通过 Remote-SSH 直接连接到 devcontainer 容器，从而复用同一套开发环境。

## 回退方案

- 删除 `.devcontainer/setup-sshd.sh`、`.devcontainer/start-sshd.sh`。
- 移除 devcontainer.json 中追加的脚本调用。
- 移除 `docker-compose.yml` 的 SSH 端口映射。
