# 2025-12-30 安装脚本修复（Ubuntu 24.04 兼容）

## 背景

安装脚本存在多处问题：验证逻辑执行顺序错误、Ubuntu 24.04 的 `externally-managed-environment` 限制导致 pip 安装失败、Docker 配置引用不存在的 Dockerfile。

## 变更

### 安装脚本重写

- `scripts/install_deps.sh`：重组结构（update → install → verify），使用 `dpkg -l` 替代 `ldconfig` 验证
- Ubuntu 24.04 兼容：优先使用 `pipx` 安装 Python 包，fallback 到 `pip3 --break-system-packages`
- 修正包名：确认 `libtbb12`、`libdeflate0` 等 Ubuntu 24.04 正确包名
- 分离运行时与开发依赖

### Docker 配置修复

- `docker-compose.yml`：修正 Dockerfile 引用路径
- `docker/Dockerfile`：添加缺失的 `libdeflate-dev`，改进生产镜像安全配置

### 文档

- 新增 `docs/installation.md` 安装指南
- 新增 `DEPLOYMENT.md` 部署指南

## 影响文件

- `scripts/install_deps.sh`、`scripts/install_runtime.sh`、`scripts/docker_deploy.sh`
- `docker/docker-compose.yml`、`docker/Dockerfile`
- `docs/installation.md`、`DEPLOYMENT.md`