# 2026-01-15 ~ 2026-01-17 Devcontainer Claude CLI 集成与配置优化

> 本文件合并自 7 个跨 3 天同主题的独立 changelog 条目。

## 1. 安装 Claude Code 与 Codex CLI（01-15）

- `docker/Dockerfile.dev`：
  - 增加 `nodejs`/`npm` 依赖
  - 安装 Claude Code 与 Codex CLI（`developer` 用户级）
  - 写入启动函数到 `/etc/profile.d` 并在 bashrc 中加载

## 2. 挂载 Claude/Codex 配置目录（01-15）

- `.devcontainer/devcontainer.json` / `devcontainer.simple.json`：
  - 新增可写挂载：`${HOME}/.claude`、`${HOME}/.codex` → `/home/developer`
  - 初始化时确保宿主侧目录存在

## 3. 调整配置同步策略为只读拷贝（01-16）

- 移除宿主机 `.claude`/`.codex` 整体挂载
- 改为挂载暂存目录 `~/.fastqtools-host-claude` / `~/.fastqtools-host-codex` → `/tmp/host-*`
- 由容器脚本 `container-setup.sh` 复制到 `/home/developer/.claude` / `.codex`
- 降低双向挂载配置冲突风险

## 4. 简化 Claude CLI 安装方式（01-16）

- `docker/Dockerfile.dev`：使用官方安装脚本 `curl -fsSL https://claude.ai/install.sh | bash`
- 移除本地路径查找与拷贝到 `/usr/local/bin` 的额外步骤

## 5. 修复 host gitconfig 挂载失败（01-16）

- `.devcontainer/devcontainer.json` / `devcontainer.simple.json`：
  - host gitconfig 临时文件从 `/tmp/fastqtools-host-gitconfig` 改为 `${localEnv:HOME}/.fastqtools-host-gitconfig`
  - `initializeCommand` 增加存在性与类型校验

## 6. 修复 Claude 登录状态同步（01-17）

- `.devcontainer/scripts/host-prepare.sh`：同步 `~/.claude/config.json` 与 `~/.claude.json`
- `.devcontainer/scripts/container-setup.sh`：从 `/tmp/host-claude/` 复制配置文件
- 容器内 Claude CLI 继承宿主机登录状态

## 7. DevContainer 简化配置使用说明（01-17）

- `.devcontainer/README.md`：补充 `devcontainer.simple.json` 的 VS Code 选择步骤与 CLI 用法

## 影响

- 开发镜像构建时间略有增加
- 仅影响 devcontainer 环境，不影响生产镜像
- 容器内 Claude/Codex 配置为只读拷贝，不与宿主机双向同步
