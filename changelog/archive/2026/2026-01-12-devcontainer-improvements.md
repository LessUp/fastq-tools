# 2026-01-12 Devcontainer 综合改进

> 本文件合并自 5 个同日同主题的独立 changelog 条目。

## 1. 修复 ~/.gitconfig 挂载失败

### 背景
VS Code "Reopen in Container" 时，`${localEnv:HOME}/.gitconfig` 在部分环境下被展开为 `/.gitconfig`（`HOME` 为空），导致 Docker 将其创建为目录并报错。

### 变更
- `.devcontainer/devcontainer.json` / `devcontainer.simple.json`
  - 新增 `initializeCommand`：在宿主机侧确保 `~/.gitconfig` 存在且为文件。
  - 调整 `mounts`：改为将 `~/.gitconfig` 复制到 `/tmp/fastqtools-host-gitconfig` 再 bind mount。

---

## 2. 远程服务器手动启动 devcontainer 脚本

### 变更
- `docker/start_devcontainer.sh`（新增）
  - 一键启动 dev 容器（build/up）
  - 可选注入 `authorized_keys`
  - 调用容器内 sshd（监听 2222）
  - 兼容 `docker compose` 与 `docker-compose`

### 使用
- 默认绑定本机：`127.0.0.1:2222`
- 异机访问：`--bind 0.0.0.0`（需配合防火墙）

---

## 3. devcontainer.simple.json 增加 SSH 端口发布

### 变更
- `.devcontainer/devcontainer.simple.json`：新增 `runArgs` 发布 `127.0.0.1:2222 -> 2222`。
- `docs/dev/windsurf-cursor-devcontainer-ssh.md`：补充端口发布说明。

---

## 4. sshd authorized_keys fallback 刷新修复

### 背景
容器内 `/home/developer/.ssh_authorized_keys` 首次创建为空或宿主机新增公钥后，fallback 文件不再刷新，导致 `Permission denied (publickey)`。

### 变更
- `.devcontainer/setup-sshd.sh`：调整为每次运行都刷新授权文件。

---

## 5. C++ 扩展与配置优化（clangd 优先）

### 变更
- `.devcontainer/devcontainer.json` / `devcontainer.simple.json`
  - 默认扩展改为：`cmake-tools` + `vscode-clangd` + `vscode-lldb`
  - 移除 `ms-vscode.cpptools` / `cpptools-extension-pack`
  - 补强 CMake/clangd 配置：Presets、`compile_commands.json` 导出、clangd 参数与 `--query-driver`

### 影响
- 默认语言服务为 clangd，跨 VS Code/Cursor/Windsurf 兼容性更好。
