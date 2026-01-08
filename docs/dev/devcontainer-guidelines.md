# Dev Container 团队开发规范（Windows + WSL2 + Docker Desktop）

本规范用于统一 FastQTools 在 Windows 环境下的容器化开发流程，目标是：**更稳定、更快、更一致、更易协作**。

## 适用范围

- Windows 11/10 + Docker Desktop（WSL2 后端）
- VS Code + Dev Containers
- 推荐使用 WSL2 的 Linux 文件系统存放仓库

## 规范（必须遵守）

### 1. 必须在 WSL2 中打开仓库

- 使用 VS Code 打开方式：
  - 通过 **Remote - WSL** 打开仓库目录（例如 `~/lessup/fastq-tools`）
  - 再执行 **Reopen in Container**

原因：容器 bind mount / 文件 IO 在 WSL2 Linux 文件系统下更稳定、性能更好，也更接近 CI/Linux 生产环境。

### 2. 必须使用 SSH（key + agent）访问私有仓库

容器内默认通过转发 **SSH Agent** 访问私有仓库，避免将私钥直接复制进容器。

- 在 WSL 中启动并加载 key：

```bash
# 启动 agent
eval "$(ssh-agent -s)"

# 加载 key（按需选择）
ssh-add
# 或 ssh-add ~/.ssh/id_ed25519

# 验证
ssh-add -l
```

### 3. Git 身份配置

容器会从宿主机（WSL）自动挂载 `~/.gitconfig`（只读）。

- 推荐在 WSL 中配置一次：

```bash
git config --global user.name "Your Name"
git config --global user.email "you@example.com"
```

## 推荐工作流

### 1) 第一次使用（新同事）

- 安装并启用：Docker Desktop（WSL2 后端）
- 在 Docker Desktop 中启用目标 WSL distro integration
- 在 WSL2 中克隆仓库（推荐）：

```bash
mkdir -p ~/lessup
cd ~/lessup
git clone git@github.com:<org>/<repo>.git fastq-tools
```

- 在 VS Code 使用 Remote - WSL 打开 `~/lessup/fastq-tools`
- 运行：**Dev Containers: Rebuild and Reopen in Container**

### 2) 日常开发

- 始终在 WSL2 打开仓库目录
- 容器内执行构建/测试/格式化：

```bash
./scripts/build.sh
./scripts/test.sh
./scripts/lint.sh
```

## 端口约定

- 开发容器不固定绑定宿主机端口。
- 如容器内服务监听 `8080`，通过 VS Code 端口转发访问。

## 常见问题（Troubleshooting）

### Q1: 容器提示未检测到 SSH agent（/ssh-agent 不存在）

- 处理：在 WSL 中执行：

```bash
eval "$(ssh-agent -s)" && ssh-add
```

然后在 VS Code 中执行：

- **Dev Containers: Rebuild and Reopen in Container**

### Q2: Git 拉取私有仓库失败（Permission denied (publickey)）

- 检查 WSL：

```bash
ssh-add -l
ssh -T git@github.com
```

- 确认 key 已加载且仓库 URL 使用 SSH 格式。

### Q3: 性能很慢

- 确认仓库位于 WSL2 Linux 文件系统（而不是 Windows 盘符下）。
- 避免在 repo 内生成超大缓存目录；缓存应使用命名卷或用户目录缓存。
