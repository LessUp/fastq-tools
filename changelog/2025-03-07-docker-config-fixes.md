# Docker 配置检查、修复与多平台优化

**日期**: 2025-03-07
**类型**: fix(docker), feat(docker)
**影响范围**: docker/, .devcontainer/

## 修复内容

### docker-compose.yml — 脚本路径错误

- `test` 服务的 `command` 从 `/workspace/scripts/test.sh` 修正为 `/workspace/scripts/core/test`
- `build` 服务的 `command` 从 `/workspace/scripts/build.sh --release` 修正为 `/workspace/scripts/core/build --release`
- 原路径指向不存在的脚本文件，导致容器启动后立即失败

### Dockerfile.deploy — 全面重写

- **基础镜像**: `ubuntu:22.04`（GCC 11/12）→ `gcc:15.2-bookworm`（GCC 15），与 dev/prod 保持一致
- **代理参数**: `HTTP_PROXY` → `DEVCONTAINER_HTTP_PROXY`，统一命名规范
- **镜像源加速**: 新增 `USE_CHINA_MIRROR` ARG 支持
- **CMake 版本**: `4.2.1` → `4.0.2`，与 dev/prod 统一
- **运行时依赖**: 最终阶段从空白 ubuntu 改为 `debian:bookworm-slim` 并安装必要运行时库（libtbb12, zlib1g, libdeflate0 等），修复二进制因缺少动态库无法运行的问题
- **安全**: 添加非 root 用户 `fastqtools`，与 Dockerfile.prod 一致
- **构建流程**: 统一使用 `build/gcc-release` 目录、ccache、Ninja 等配置
- **Clang 21 移除**: 部署镜像无需 Clang 工具链，减小镜像体积

### devcontainer.json — SSH_AUTH_SOCK mount 导致容器创建失败

- 移除 `SSH_AUTH_SOCK` bind mount：当环境变量未设置时（Windows 无 ssh-agent、原生 Linux 未启动 agent），`${localEnv:SSH_AUTH_SOCK}` 展开为空字符串，Docker bind mount 失败，容器无法创建
- 移除 `remoteEnv` 中硬编码的 `SSH_AUTH_SOCK: "/ssh-agent"`（已无对应 mount）
- VS Code 内置 SSH agent 转发机制可自动处理，无需手动 mount

### container-setup.sh — WSL 检查误告

- 原逻辑在所有非 WSL 环境（包括原生 Linux 服务器）都会触发 WSL 建议警告
- 修改为仅在检测到 Windows Docker Desktop 环境（`/mnt/c/Windows` 存在且无 WSL_DISTRO_NAME）时提示
- 同时移除已无效的 SSH agent socket 检查

### .devcontainer/README.md — docker/ 文件树不完整

- 补全 `Dockerfile.prod`、`Dockerfile.deploy`、`data/`、`output/` 条目

### docker-compose.yml — prod 服务多余配置

- 移除 `ports: "8080:8080"`（FastQTools 是 CLI 工具，无 HTTP 服务）
- 移除 `restart: unless-stopped`（CLI 工具运行完即退出，持续重启无意义）

## 多平台优化 (WSL2 / 远程 Linux / Windows)

### host-prepare.sh — 平台检测与自适应

- 新增 `detect_platform()` 函数：自动识别 WSL2、原生 Linux、Windows Git Bash
- 各平台启动时给出针对性提示（WSL2 推荐、Windows 原生警告、Linux 正常）
- WSL2 场景：检测项目是否在 `/mnt/*` 路径下并警告 volume 性能问题
- 自动从 `docker/.env` 读取数据路径并创建目录

### devcontainer.json — 平台感知环境变量

- 新增 `MSYSTEM` remoteEnv 传递，配合 `WSL_DISTRO_NAME` 让容器内精确判断宿主机平台
- 移除无用的 `forwardPorts: [8080]`（CLI 工具无 HTTP 服务）

### container-setup.sh — 平台感知环境检查

- 新增 `detect_host_platform()` 函数：基于 remoteEnv 传入的 `WSL_DISTRO_NAME` / `MSYSTEM` 精确判断
- `check_environment()` 按平台分别给出 info/warn 级别提示，不再对原生 Linux 误报

### .env.example — 分平台配置示例

- 按 WSL2/Windows Docker Desktop 和远程 Linux 服务器分区提供代理配置示例
- 明确说明 `host.docker.internal` 在远程 Linux 上默认不可用及替代方案
- SSH 绑定地址增加远程场景说明

### start_devcontainer.sh — 前置检查增强

- 新增 Docker 可用性检查（未运行或无权限时给出明确错误信息）
- 新增 `.env` 文件存在性检查（缺失时提示复制模板）
- 自动创建数据挂载目录

### README.md — 三平台完整指南

- 新增环境对比表（WSL2 ⭐⭐⭐ / 远程 Linux ⭐⭐⭐ / Windows 原生 ⚠️）
- 按平台提供独立的快速开始步骤
- 分平台网络代理配置说明
- 新增 WSL2 volume 性能、Docker 未运行等故障排除条目
