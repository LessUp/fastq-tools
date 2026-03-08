# 2026-01-08 DevContainer 综合修复

> 本文件合并自 4 个同日同主题的独立 changelog 条目。

## 背景

DevContainer 在多个场景下存在问题：Conan 版本过旧导致 GCC 15 不被识别、Git 配置文件被创建为目录、VS Code 打开时多处报错、Windows Docker Desktop 环境下启动失败。

## 变更

### 1. Conan 版本升级

- `docker/Dockerfile.dev`：Conan 从 `2.14.0` 升级到 `2.24.0`，与 CI/生产镜像一致
- 修复 `conan install` 因 `settings.compiler.version=15` 不被识别而失败

### 2. Git 配置修复

- `devcontainer.json` 增加 `postStartCommand`/`postAttachCommand`：每次启动修复 `.gitconfig` 目录问题、设置 `safe.directory`
- 禁用 Dev Containers 内置 Git 配置复制（`remote.containers.copyGitConfig=false`）
- `docker/Dockerfile.dev`：预创建 `.gitconfig` 为普通文件，系统级预写 `safe.directory=/workspace`
- 修正 `scripts/core/*` 对 `lib/common.sh` 的引用路径和可执行权限

### 3. VS Code 打开报错修复

- `devcontainer.json`：命令末尾增加 `true`，避免非关键步骤导致整体失败
- 移除 `sudo` 依赖，改用 `git config --global`
- `docker/Dockerfile.dev`：安装 `skopeo`，消除 devcontainers helper 探测返回 127

### 4. Windows 兼容性修复

- `docker-compose.yml`：移除 `version` 字段、改用命名卷替代 `${HOME}` 挂载、移除端口直绑
- `devcontainer.json`：增加 `runServices: ["dev"]`、`forwardPorts: [8080]`、SSH/Git 自动注入
- 新增 `docs/dev/devcontainer-guidelines.md`

## 影响

- 需要 "Rebuild/Reopen in Container" 生效
- Windows + Docker Desktop（WSL2 后端）下启动不再因端口占用或 HOME 变量缺失而失败
