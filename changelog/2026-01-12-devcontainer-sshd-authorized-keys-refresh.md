# Devcontainer sshd：authorized_keys fallback 刷新修复（2026-01-12）

## 背景

devcontainer 内通过 sshd 供 Windsurf/Cursor/Remote-SSH 连接时，服务端使用容器内可写的 `/home/developer/.ssh_authorized_keys` 作为授权文件（避免只读挂载 `~/.ssh`、权限/属主问题导致 sshd 忽略 key）。

此前逻辑在该 fallback 文件已存在时会直接退出，导致首次创建为空文件或后续宿主机新增公钥后，容器内授权文件不再刷新，从而出现 `Permission denied (publickey)`。

## 变更

- `.devcontainer/setup-sshd.sh`
  - 调整为每次运行都刷新 `/home/developer/.ssh_authorized_keys`：
    - 优先复制挂载的 `/home/developer/.ssh/authorized_keys`（若非空）
    - 否则收集挂载的 `id_*.pub` 追加生成

## 影响

- Remote-SSH/Windsurf/Cursor 连接更稳定：宿主机侧公钥变化后无需手动删除 fallback 文件。

## 回退方案

- 回退 `.devcontainer/setup-sshd.sh` 中对 fallback 刷新逻辑的修改。
