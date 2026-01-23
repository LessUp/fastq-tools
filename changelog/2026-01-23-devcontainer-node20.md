# Devcontainer Node.js 升级至 20.x（2026-01-23）

## 背景

在 dev 容器内运行 `npx zcf` 触发 `SyntaxError: Invalid regular expression flags`，原因是依赖包要求 Node >=20，但容器使用了 Debian 默认的 Node 18。

## 变更

- `docker/Dockerfile.dev`
  - 移除 apt 默认的 `nodejs/npm`。
  - 增加 NodeSource 安装逻辑，默认安装 Node.js 20.x。

## 影响

- `npx zcf` 等现代 CLI 工具可直接使用，不再受 Node 18 限制。
