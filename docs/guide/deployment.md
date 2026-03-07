# 部署指南

本文档涵盖 FastQTools 在 Docker 和生产环境中的部署方案。

---

## Docker 部署

### 生产环境

```bash
# 构建生产镜像
./scripts/tools/deploy --env production --action build

# 运行
./scripts/tools/deploy --env production --action run

# 或使用 docker-compose
docker-compose -f docker/docker-compose.yml up -d prod
```

### 开发环境

```bash
# 运行开发容器
./scripts/tools/deploy --env development --action run

# 或使用 VS Code DevContainer
# Ctrl+Shift+P → "Reopen in Container"
```

### CI/CD

```bash
# 构建 + 测试
./scripts/tools/deploy --env test --action run

# 推送到镜像仓库
./scripts/tools/deploy --action push --registry your-registry.com/fastqtools --tag v3.1.0
```

---

## 容器镜像

### 生产镜像 (`docker/Dockerfile.prod`)

- **基础**：debian:12-slim
- **大小**：~100MB
- **内容**：FastQTools 二进制 + 运行时库
- **用户**：非 root（fastqtools:1000）

### 开发镜像 (`docker/Dockerfile.dev`)

- **基础**：gcc:14.2-bookworm
- **大小**：~2GB
- **内容**：完整开发工具链
- **用户**：非 root（developer:1000）

---

## 卷挂载

### 生产

```yaml
volumes:
  - ./data:/app/data:ro           # 只读输入
  - ./output:/app/output          # 可写输出
```

### 开发

```yaml
volumes:
  - ..:/workspace:cached          # 源码
  - ~/.conan2:/home/developer/.conan2:cached  # Conan 缓存
  - ~/.ccache:/home/developer/.ccache:cached  # 构建缓存
```

---

## 安全性

### 生产

- 非 root 用户运行（UID 1000）
- 最小化基础镜像
- 只读输入卷
- 健康检查：`fastqtools --help || exit 1`

### 开发

- 非 root + sudo
- SSH Agent 转发（Git 操作）
- Git 配置持久化

---

## 性能优化

- **ccache**：增量构建缓存
- **Ninja**：并行构建
- **Conan 缓存**：避免重复编译依赖
- **多阶段构建**：减小镜像体积
- **Release 构建**：`-O3 -march=native`

---

## 健康检查与日志

```bash
# 健康检查
docker exec fastqtools-prod fastqtools --help

# 查看日志
docker logs fastqtools-prod
docker logs -f fastqtools-prod  # 实时跟踪
```

---

## 常见问题

### Permission Denied

```bash
sudo chown -R 1000:1000 ./data ./output
```

### 缺少依赖库

```bash
docker exec fastqtools-prod ldd /usr/local/bin/FastQTools
```

### 调试模式

```bash
docker run -it --rm fastqtools:latest /bin/bash
docker exec -it fastqtools-prod /bin/bash
```

---

## 版本更新

1. 构建新版本镜像
2. 停止旧容器
3. 使用相同卷启动新容器
4. 验证功能
5. 删除旧镜像

---

## 最佳实践

- 使用具体版本标签，不用 `latest`
- 设置 CPU/内存资源限制
- 尽可能使用只读根文件系统
- 定期更新基础镜像
- 监控容器资源使用
