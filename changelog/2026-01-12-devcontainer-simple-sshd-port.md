# devcontainer.simple.json 增加 SSH 端口发布以支持 Windsurf/Cursor（2026-01-12）

## 变更

- `.devcontainer/devcontainer.simple.json`
  - 新增 `runArgs`：发布 `127.0.0.1:2222 -> 2222`，使本机可通过 SSH 直接连接 devcontainer（适配 Windsurf/Cursor Remote-SSH 工作流）。
- `docs/dev/windsurf-cursor-devcontainer-ssh.md`
  - 补充 `devcontainer.simple.json` 场景下的端口发布说明与修改指引。

## 影响

- VS Code Dev Containers 使用方式不变。
- Windsurf/Cursor 在无法“容器内 Reopen in Container”的场景下，可通过 SSH 直连容器复用同一套工具链。

## 回退方案

- 移除 `.devcontainer/devcontainer.simple.json` 的 `runArgs`。
- 回退文档中关于 `runArgs` 的说明。
