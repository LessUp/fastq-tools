# 生产镜像 Dockerfile 重命名（2026-01-23）

## 变更
- 将 `docker/Dockerfile` 重命名为 `docker/Dockerfile.prod`。
- 同步更新 `docker-compose.yml`、CI 与部署脚本中对生产 Dockerfile 的引用。
- 文档中生产镜像路径更新为 `docker/Dockerfile.prod`。

## 影响
- 生产镜像构建需使用新的 Dockerfile 路径，相关脚本与 CI 已同步。
