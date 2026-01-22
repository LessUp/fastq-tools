# Devcontainer 数据目录挂载可配置化（2026-01-23）

## 变更
- `docker/docker-compose.yml`：dev 服务新增 `/data` 绑定宿主机目录，支持 `FASTQTOOLS_HOST_DATA_PATH` 配置。
- `docker/.env` 与 `docker/.env.example`：新增宿主机数据目录变量说明。
- `docker/start_devcontainer.sh`：支持 `--data-path` 与环境变量覆盖挂载路径。
- `.devcontainer/devcontainer.simple.json`：增加 `/data` 绑定宿主机目录。
- `.devcontainer/README.md`：补充配置说明与示例。

## 影响
- 通过 `FASTQTOOLS_HOST_DATA_PATH` 可在 WSL/远程服务器上统一配置宿主机数据目录挂载。
- 简化配置需在宿主机环境显式设置该变量，否则 `/data` 绑定失败。
