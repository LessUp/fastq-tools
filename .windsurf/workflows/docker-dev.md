---
title: "/docker-dev"
description: "Docker 开发环境管理：构建/启动/进入/清理 DevContainer，快速搭建一致的开发环境。"
auto_execution_mode: 1
---

## 适用场景
- 首次搭建开发环境
- DevContainer 出问题需要重建
- 切换不同编译器/工具链版本

## 工作流步骤
1. **宿主机准备**
   - `bash .devcontainer/scripts/host-prepare.sh`
   - 确认 Docker 已安装并运行：`docker info`

2. **构建开发镜像**
   - `docker build -f docker/Dockerfile.dev -t fastqtools-dev:latest .`
   - 或使用 docker-compose：`docker-compose -f docker/docker-compose.yml build dev`

3. **启动开发容器**
   - **VS Code DevContainer（推荐）**：
     - Ctrl+Shift+P → "Reopen in Container"
   - **手动启动**：
     - `docker-compose -f docker/docker-compose.yml up -d dev`
     - `docker exec -it fastqtools-dev bash`

4. **容器内初始化**
   - `./scripts/tools/setup-devcontainer`
   - 验证工具链：
     ```bash
     clang++ --version
     cmake --version
     ninja --version
     conan --version
     ```

5. **容器内开发**
   - 构建：`./scripts/core/build -c clang -t Debug`
   - 测试：`./scripts/core/test -c clang -t Debug`
   - lint：`./scripts/core/lint format-check`

6. **清理（需要时）**
   - 停止容器：`docker-compose -f docker/docker-compose.yml down`
   - 删除镜像：`docker rmi fastqtools-dev:latest`
   - 清理构建缓存：`docker builder prune`

## 备注
- DevContainer 配置在 `.devcontainer/devcontainer.json`
- Docker 配置在 `docker/` 目录
- 容器内数据通过 volume 持久化，重启不丢失
