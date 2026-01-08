# Dev Container Git 配置修复（2026-01-08）

## 背景
- 在 Dev Container 中执行 `git status` 触发两类错误：
  - `warning: unable to access '/home/developer/.gitconfig': Is a directory`，导致 `git config --global ...` 失败。
  - `fatal: detected dubious ownership in repository at '/workspace'`，Git 认为工作区不安全。

## 根因
- `~/.gitconfig` 在某些场景会被异常创建成目录（例如挂载源不存在被 Docker 创建为目录），随后 Git 无法读取全局配置文件。
- Dev Container 仅在首次创建时执行 `postCreateCommand`，后续重连/重启时不一定会修复该状态。

## 本次变更
- 更新 `.devcontainer/devcontainer.json`
  - 增加 `postStartCommand`：每次启动/附加时
    - 若 `/home/developer/.gitconfig` 是目录则删除并重建为文件；
    - 若存在 `/tmp/host-gitconfig` 则复制到 `/home/developer/.gitconfig`；
    - 使用 `git config --system --add safe.directory /workspace` 确保 `/workspace` 被 Git 信任。
  - 禁用 Dev Containers 内置的 Git 配置复制：`remote.containers.copyGitConfig=false`，避免启动早期阶段因 `/home/developer/.gitconfig` 被异常创建为目录而直接失败。

- 更新 `docker/Dockerfile.dev`
  - 预创建 `/tmp/host-gitconfig`，避免后续流程依赖该路径时出现异常。
  - 在镜像构建阶段初始化 `/home/developer/.gitconfig` 为普通文件。
  - 在系统级 Git 配置中预写入 `safe.directory=/workspace`，避免 `dubious ownership`。

- 更新 `scripts/core/*`
  - 修正核心脚本对 `scripts/lib/common.sh` 的引用路径，避免 `source` 到不存在的 `scripts/core/lib/common.sh`。

## 使用说明
- 需要 `Rebuild/Reopen in Container` 让镜像与 Dev Container 配置生效。
