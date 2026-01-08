# Devcontainer Windows 兼容性修复（2026-01-08）

## 变更
- `docker/docker-compose.yml`：
  - 移除已废弃的 `version` 字段（Compose V2 会忽略并报警告）。
  - `dev/test/build` 服务不再依赖宿主机 `${HOME}` 路径挂载，改为使用命名卷保存 Conan/ccache 缓存（跨平台更稳定）。
  - `dev` 服务不再将容器端口 `8080` 绑定到宿主机端口（避免在 Windows 上出现端口已占用导致 Dev Container 启动失败）。
- `.devcontainer/devcontainer.json`：
  - 增加 `runServices: ["dev"]`，确保 Dev Container 仅拉起开发服务，避免默认 `compose up` 启动所有服务。
  - 增加 `forwardPorts: [8080]`，通过 VS Code 端口转发访问容器内服务。
  - 增加对 WSL2 打开方式的校验：要求通过 VS Code Remote - WSL 打开工作区后再 Reopen in Container。
  - 增加 SSH/Git 自动注入：挂载宿主机（WSL）侧 `~/.ssh` 与 `~/.gitconfig`，并转发 `SSH_AUTH_SOCK`。
  - 将 WSL2/SSH 校验由“强制失败”调整为“仅提示”（warning），避免将工作流规定写死为硬阻断。
  - 修复 `.gitconfig` 注入导致容器内路径变成目录的问题：改为挂载到临时路径并在创建阶段复制到用户目录；同时避免在非 Git 仓库目录执行 `git config` 报错。
- `scripts/devcontainer_setup.sh`：修复在 `set -u` 模式下引用未定义环境变量（`GIT_USER_NAME` / `GIT_USER_EMAIL`）导致脚本退出的问题。
- 文档：新增 `docs/dev/devcontainer-guidelines.md` 作为团队 Dev Container 开发规范，并在 `README.md` / `docs/README.md` 中增加入口链接。

## 影响
- 在 Windows + Docker Desktop（WSL2 后端）下启动 Dev Container 时，不再因宿主机 `8080` 被占用而失败。
- 不再依赖 Windows 侧 `HOME` 环境变量，减少 `"HOME variable is not set"` 相关警告与挂载失败风险。

## 使用
- 在 VS Code 中执行 “Dev Containers: Rebuild and Reopen in Container”。
- 如需访问容器内 `8080` 服务，使用 VS Code 的端口转发（Ports 面板）。
- 如确实需要宿主机直出端口，请自行在 `docker-compose.yml` 的 `dev` 服务中添加 `ports` 映射，并确保宿主机端口未被占用。
- 需在 WSL 中准备好 SSH Agent（示例：`eval "$(ssh-agent -s)" && ssh-add`），否则容器创建阶段会失败并提示缺少 `/ssh-agent` socket。
