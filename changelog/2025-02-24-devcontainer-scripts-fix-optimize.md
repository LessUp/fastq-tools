# 2025-02-24 修复与优化 DevContainer 环境脚本

## 背景
- DevContainer 脚本存在多个 Bug 和可优化项，影响容器启动可靠性和开发体验。

## 变更

### Bug 修复

1. **`setup-sshd.sh` — `collect_keys()` 返回值逻辑反转**
   - Bash 约定 `return 0` = 成功，`return 1` = 失败。原代码 `return $keys_found` 在找到 key 时返回 1（失败），导致成功/失败日志完全颠倒。
   - 修复为显式 `return 0`/`return 1`。

2. **`devcontainer.simple.json` — `SSH_AUTH_SOCK` 挂载导致启动失败**
   - 当宿主机未设置 `SSH_AUTH_SOCK` 环境变量时，bind mount 会失败并阻止容器启动。
   - 移除了该硬挂载项。

3. **`host-prepare.sh` — 缺少 `~/.ssh` 目录检查**
   - `devcontainer.json` 挂载 `~/.ssh`，但 `host-prepare.sh` 未确保该目录存在。
   - 添加 `ensure_dir "$H/.ssh"` 及权限设置。

### 优化

4. **`container-setup.sh` — SSH agent 检查逻辑改进**
   - 原逻辑在 docker-compose 模式下永远打印 SSH agent 警告（因为没有挂载 `/ssh-agent`）。
   - 改为仅在 `/ssh-agent` 路径存在但不可用时才警告。

5. **`container-setup.sh` — 消除 `cmd_attach`/`cmd_start` 重复**
   - 提取共用逻辑到 `_sync_and_start()` 函数。

6. **`host-prepare.sh` — 简化 gitconfig 冗余操作**
   - 移除了 `ensure_file` + `cp -f` 的冗余组合，改为先清理目录类型错误再直接复制。

7. **`start-sshd.sh` — 哨兵文件避免重复 setup**
   - 使用 `/tmp/.sshd-setup-done` 哨兵文件，仅在首次或 `setup-sshd.sh` 更新后重新执行配置。

## 影响
- 不影响 Dockerfile 或 docker-compose.yml。
- 修复了可能导致容器启动失败的关键问题。
- 减少了容器 start/attach 阶段的冗余操作。
