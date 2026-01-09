# Dev Container 打开报错修复（2026-01-08）

## 背景
- VS Code Dev Containers 打开/Attach 时日志中出现多处非 0 退出码：
  - `Exit code 127`：在容器内执行 `(command -v 'docker' || command -v 'oras' || command -v 'skopeo')` 失败。
  - `postAttachCommand`/`postStartCommand` 存在 `sudo` + `git config --system` 的组合，容易在容器权限/环境差异时导致命令失败，从而影响 Attach 流程。

## 根因
- `docker/oras/skopeo` 均未安装时，devcontainers helper 的能力探测返回 127。
- `postStartCommand`/`postAttachCommand` 依赖 `sudo` 和 system 级 git 配置，在不同镜像/权限策略下可能失败（并导致 Dev Containers 认为 post-attach 流程失败）。

## 本次变更
- 更新 `.devcontainer/devcontainer.json`
  - `postCreateCommand`/`postStartCommand`/`postAttachCommand`：
    - 不再依赖 `sudo`。
    - 使用 `git config --global --add safe.directory /workspace` 规避 `dubious ownership`。
    - 命令末尾增加 `true`，避免非关键步骤导致整体失败。

- 更新 `.devcontainer/devcontainer.simple.json`
  - 同步上述逻辑，确保 simple 配置行为一致。

- 更新 `docker/Dockerfile.dev`
  - 安装 `skopeo`，使 devcontainers helper 的 `(docker|oras|skopeo)` 探测不再返回 127。

## 使用说明
- 需要执行 `Rebuild/Reopen in Container` 让镜像与 Dev Container 配置生效。
