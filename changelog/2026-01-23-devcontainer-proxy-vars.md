# Devcontainer 代理变量通用化（2026-01-23）

## 变更
- `docker/Dockerfile.dev`：代理变量统一为 `DEVCONTAINER_*`，仅显式设置时写入 apt 代理。
- `docker/docker-compose.yml`：dev build args 改为 `DEVCONTAINER_*`。
- `docker/.env` 与 `docker/.env.example`：更新代理示例变量名。
- `.devcontainer/README.md`：同步代理示例。

## 影响
- 代理配置命名更通用，便于跨项目复用；默认构建不再继承宿主机 `HTTP_PROXY/HTTPS_PROXY`。
