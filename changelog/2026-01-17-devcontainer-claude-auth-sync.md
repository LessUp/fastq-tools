# 修复 Devcontainer Claude 登录状态同步（2026-01-17）

## 背景

容器内运行 `claude-minimax` 时出现登录方式选择界面，而宿主机能直接进入信任目录提示。排查发现容器内缺少 Claude CLI 的登录状态文件与部分配置文件。

## 变更

- `.devcontainer/scripts/host-prepare.sh`
  - 同步 `~/.claude/config.json`。
  - 新增同步 `~/.claude.json` 到宿主机暂存目录 `~/.fastqtools-host-claude/.claude.json`。
- `.devcontainer/scripts/container-setup.sh`
  - 同步 `config.json` 到 `/home/developer/.claude/`。
  - 从 `/tmp/host-claude/.claude.json` 复制到 `/home/developer/.claude.json`。

## 影响

- 容器内 Claude CLI 继承宿主机登录状态与配置，保持与 WSL2 端一致的启动体验。

## 回退方案

- 回退上述脚本对 `config.json` 与 `.claude.json` 的同步逻辑。
