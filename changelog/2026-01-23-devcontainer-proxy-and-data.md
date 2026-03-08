# 2026-01-22 ~ 2026-01-23 Devcontainer 代理与数据目录配置

> 本文件合并自 3 个跨 2 天同主题的独立 changelog 条目。

## 1. 代理变量修复（01-22）

- `docker/Dockerfile.dev`：改用 `FASTQTOOLS_*` 代理参数，仅显式设置时写入 apt 代理。
- `docker/docker-compose.yml`：build args 改为 `FASTQTOOLS_*`，避免宿主机 `HTTP_PROXY/HTTPS_PROXY` 自动透传。
- `docker/.env` 与 `docker/.env.example`：更新代理示例变量名。
- `.devcontainer/README.md`：同步代理示例。

## 2. 代理变量通用化（01-23）

- 代理变量从 `FASTQTOOLS_*` 统一为 `DEVCONTAINER_*`，便于跨项目复用。
- 同步更新 `Dockerfile.dev`、`docker-compose.yml`、`.env`、`.env.example`、`README.md`。

## 3. 数据目录挂载可配置化（01-23）

- `docker/docker-compose.yml`：dev 服务新增 `/data` 绑定宿主机目录，支持 `FASTQTOOLS_HOST_DATA_PATH` 配置。
- `docker/.env` 与 `docker/.env.example`：新增宿主机数据目录变量说明。
- `docker/start_devcontainer.sh`：支持 `--data-path` 与环境变量覆盖挂载路径。
- `.devcontainer/devcontainer.simple.json`：增加 `/data` 绑定。
- `.devcontainer/README.md`：补充配置说明与示例。

## 影响

- 默认构建不再尝试连接本地代理；需要代理时在 `docker/.env` 设置 `DEVCONTAINER_*`。
- 通过 `FASTQTOOLS_HOST_DATA_PATH` 可配置宿主机数据目录挂载。
