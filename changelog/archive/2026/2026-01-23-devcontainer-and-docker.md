# 2026-01-23 DevContainer 与 Docker 配置更新

> 本文件合并自 3 个同日同主题的独立 changelog 条目。

## 背景

Dev 容器内 Node.js 版本过旧、代理变量命名不统一、数据目录挂载不可配置、生产 Dockerfile 命名不规范。

## 变更

### 1. Node.js 升级至 20.x

- `docker/Dockerfile.dev`：移除 apt 默认 Node 18，使用 NodeSource 安装 Node.js 20.x
- 修复 `npx zcf` 等现代 CLI 工具因 Node 版本不足报错

### 2. 代理变量统一与数据目录配置

- 代理变量从 `FASTQTOOLS_*` 统一为 `DEVCONTAINER_*`，便于跨项目复用
- `docker-compose.yml` dev 服务新增 `/data` 绑定，支持 `FASTQTOOLS_HOST_DATA_PATH` 配置
- `docker/start_devcontainer.sh` 支持 `--data-path` 参数
- 同步更新 `.env`、`.env.example`、`README.md`

### 3. 生产镜像 Dockerfile 重命名

- `docker/Dockerfile` → `docker/Dockerfile.prod`
- 同步更新 `docker-compose.yml`、CI 与部署脚本中的引用

## 影响

- 默认构建不再尝试连接本地代理
- 生产镜像构建需使用 `docker/Dockerfile.prod`
