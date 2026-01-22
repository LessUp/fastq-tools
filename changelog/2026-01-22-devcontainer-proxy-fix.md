# Devcontainer 代理变量修复（2026-01-22）

## 变更
- `docker/Dockerfile.dev`：改用 `FASTQTOOLS_*` 代理参数，仅显式设置时写入 apt 代理。
- `docker/docker-compose.yml`：build args 改为 `FASTQTOOLS_*`，避免宿主机 `HTTP_PROXY/HTTPS_PROXY` 自动透传。
- `docker/.env` 与 `docker/.env.example`：更新代理示例变量名。
- `.devcontainer/README.md`：同步代理示例。

## 影响
- 默认构建不再尝试连接本地 `127.0.0.1:10808` 代理；需要代理时在 `docker/.env` 设置 `FASTQTOOLS_*`。
