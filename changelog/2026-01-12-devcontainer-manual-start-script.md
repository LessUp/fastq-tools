# 远程服务器手动启动 devcontainer 脚本（2026-01-12）

## 背景

在远程服务器上不使用 VS Code / Dev Containers 扩展时，仍希望复用仓库的开发容器（dev service）并通过 sshd 提供给 Windsurf/Cursor/Remote-SSH 连接。

## 变更

- `docker/start_devcontainer.sh`
  - 新增：一键启动 dev 容器（build/up）
  - 新增：可选注入 `authorized_keys` 到容器内 `developer` 用户
  - 新增：调用 `/workspace/.devcontainer/start-sshd.sh` 启动容器内 sshd（监听 2222）
  - 兼容：自动选择 `docker compose` 或 `docker-compose`

## 使用说明

- 默认仅绑定本机：`127.0.0.1:2222`（更安全）
- 异机访问可用：`--bind 0.0.0.0`（需配合防火墙限制来源）

## 影响

- 在不依赖 VS Code 的情况下，也可以在远程服务器上启动一致的开发容器环境并通过 Remote-SSH 连接。

## 回退方案

- 删除 `docker/start_devcontainer.sh`
