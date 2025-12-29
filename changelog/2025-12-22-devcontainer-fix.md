# Devcontainer 修复与升级（2025-12-22）

## 变更
- `docker/Dockerfile.dev`：改为自包含基础镜像，安装构建链与 Conan，添加开发工具，创建 developer 用户并设置工作目录。
- `docker/docker-compose.yml`：开发容器暴露 8080 端口，保持与 VS Code 端口转发一致。
- `.devcontainer/devcontainer.json`：指定远程用户 `developer`，避免权限问题。

## 影响
- VS Code Dev Container 可直接构建并进入 `developer` 用户，无需依赖主 `Dockerfile` 的 stage。
- 端口 8080 转发保持可用。

## 使用
- 在 VS Code 选择 “Rebuild and Reopen in Container” 重新构建。
- 初次进入后自动执行 `scripts/devcontainer_setup.sh` 完成工具配置。
