# DevContainer 配置优化

**日期**: 2026-02-25

## 变更内容

### 删除冗余配置
- 删除 `devcontainer.simple.json`，统一使用 docker-compose 模式

### 文件组织优化
- 将 `setup-sshd.sh` 和 `start-sshd.sh` 从 `.devcontainer/` 根目录移至 `.devcontainer/scripts/`，与其他脚本统一管理
- 更新所有引用路径：`container-setup.sh`、`start-sshd.sh`、`docker/start_devcontainer.sh`、`HELP.md`、`docs/dev/devcontainer-ssh.md`

### 增强 devcontainer.json（从 fq-compressor 前向移植）
- 添加 `features`（common-utils: zsh/oh-my-zsh）
- 添加 SSH_AUTH_SOCK 转发（mounts + remoteEnv）
- 添加 `portsAttributes` 端口标签
- 添加 `EDITOR`/`VISUAL` 环境变量
- 添加更多 clangd 参数：`--all-scopes-completion`、`--pch-storage=memory`、`clangd.checkUpdates`
- 添加编辑器设置：`formatOnSave`、文件关联（.h/.hpp → cpp）、`trimTrailingWhitespace`、`insertFinalNewline`
- 添加搜索排除：build/cmake-build-*/.conan2/.ccache
- 添加 `cmake.buildDirectory` 配置
- 添加更多 VS Code 扩展：twxs.cmake、better-cpp-syntax、spell-checker、EditorConfig、Docker、GitLens、git-graph、markdown-all-in-one、vscode-yaml
- 修复端口转发重复：docker-compose 已映射 2222 端口，从 `forwardPorts` 中移除

### 健壮性提升
- `container-setup.sh` 中 `WORKSPACE` 使用环境变量 fallback（`${WORKSPACE:-/workspace}`）

### 文档更新
- 更新 `README.md`：移除 simple.json 相关说明、更新文件树结构、更新 SSHD 脚本路径
- 更新 `docs/dev/devcontainer-ssh.md`：更新脚本路径、移除 simple.json 引用

## 影响范围
- `.devcontainer/devcontainer.json`
- `.devcontainer/devcontainer.simple.json`（已删除）
- `.devcontainer/scripts/setup-sshd.sh`（从根目录移入）
- `.devcontainer/scripts/start-sshd.sh`（从根目录移入）
- `.devcontainer/scripts/container-setup.sh`
- `.devcontainer/README.md`
- `docker/start_devcontainer.sh`
- `HELP.md`
- `docs/dev/devcontainer-ssh.md`
