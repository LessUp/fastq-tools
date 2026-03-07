# DevContainer 开发环境

本文档统一说明 FastQTools 在 DevContainer 中的开发环境搭建，包括 VS Code 原生支持和 Windsurf/Cursor 的 SSH 连接方案。

---

## 适用范围

- Windows 11/10 + Docker Desktop（WSL2 后端）
- VS Code + Dev Containers 扩展
- Windsurf / Cursor（通过 SSH 连接）

---

## 环境准备

### 1. 仓库必须在 WSL2 中打开

容器 bind mount 在 WSL2 Linux 文件系统下更稳定、性能更好：

```bash
# WSL2 中克隆仓库
mkdir -p ~/lessup
cd ~/lessup
git clone git@github.com:<org>/<repo>.git fastq-tools
```

VS Code 中通过 **Remote - WSL** 打开 `~/lessup/fastq-tools`，再执行 **Reopen in Container**。

### 2. SSH Agent 配置

容器通过 SSH Agent 转发访问私有仓库（不将私钥复制进容器）：

```bash
# WSL 中启动 agent 并加载 key
eval "$(ssh-agent -s)"
ssh-add ~/.ssh/id_ed25519

# 验证
ssh-add -l
ssh -T git@github.com
```

### 3. Git 身份配置

容器会从宿主机 WSL 自动挂载 `~/.gitconfig`（只读）：

```bash
git config --global user.name "Your Name"
git config --global user.email "you@example.com"
```

---

## VS Code DevContainer

两种配置可选：

1. **完整配置** (`.devcontainer/devcontainer.json`)：使用 docker-compose
2. **简易配置** (`.devcontainer/devcontainer.simple.json`)：单 Dockerfile

两者均提供：Clang/LLVM 21、CMake、Ninja、Conan、VS Code C++ 扩展。

### 日常开发

```bash
# 容器内执行
./scripts/core/build
./scripts/core/test
./scripts/core/lint
```

---

## Windsurf / Cursor SSH 方案

Windsurf/Cursor 不完全支持 "Reopen in Container"。解决方案是在 DevContainer 内启动 `sshd`，通过 SSH 直连。

### 实现方式

采用 DevContainer lifecycle hooks + sshd 自行 daemonize：

- `postCreateCommand`：运行 `setup-sshd.sh` 一次性配置
- `postStartCommand`：运行 `start-sshd.sh` 每次启动拉起 sshd

**sshd 配置**：端口 `2222`、禁用密码登录、禁用 root 登录、限制用户 `developer`。

### 连接步骤

#### 本机开发

1. 用 VS Code Dev Containers 启动容器（或 `devcontainer` CLI）
2. 确认 `postStartCommand` 已执行（sshd 自动启动）
3. 测试连接：

```bash
ssh -p 2222 developer@127.0.0.1
```

4. 在 Windsurf/Cursor 中使用 Remote-SSH：
   - Host: `127.0.0.1`
   - Port: `2222`
   - User: `developer`
   - 工作目录: `/workspace`

#### 远程主机

1. 在远程主机上启动 DevContainer
2. 设置 `FASTQTOOLS_SSH_BIND=0.0.0.0`（注意防火墙）
3. 本地 Windsurf/Cursor 直连 `remote-host:2222`

### 端口配置

通过环境变量自定义：

- `FASTQTOOLS_SSH_BIND`（默认 `127.0.0.1`）
- `FASTQTOOLS_SSH_PORT`（默认 `2222`）

---

## 快速命令参考

```bash
# 手动启动容器
docker start fastqtools-dev

# 手动启动 sshd
docker exec fastqtools-dev bash /workspace/.devcontainer/scripts/start-sshd.sh

# SSH 连接
ssh -p 2222 developer@localhost
```

---

## 常见问题

### SSH Agent 未检测到

```bash
# WSL 中执行
eval "$(ssh-agent -s)" && ssh-add

# 然后在 VS Code 中 Rebuild and Reopen in Container
```

### Git Permission denied (publickey)

```bash
ssh-add -l          # 确认 key 已加载
ssh -T git@github.com  # 测试连接
```

### SSH Connection refused

- 容器未启动或 `postStartCommand` 未执行
- 检查端口映射：`docker port fastqtools-dev`

### SSH Permission denied (publickey)

- 检查容器内 `/home/developer/.ssh_authorized_keys` 是否存在且权限为 `0600`

### 性能很慢

- 确认仓库位于 WSL2 Linux 文件系统（不是 Windows 盘符下）
- 缓存使用命名卷或用户目录

---

## 安全建议

- sshd 默认只绑定 `127.0.0.1`
- 对外开放时配合防火墙
- 已禁用密码登录，仅公钥认证
