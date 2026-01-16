# 简化 Devcontainer Claude CLI 安装方式（2026-01-16）

## 背景

原先在开发镜像中安装 Claude CLI 时增加了多余的路径探测与复制逻辑，使用上不如官方安装脚本稳定。

## 变更

- `docker/Dockerfile.dev`
  - 使用官方命令直接安装 Claude CLI：`curl -fsSL https://claude.ai/install.sh | bash`。
  - 移除额外的本地路径查找与拷贝到 `/usr/local/bin` 的步骤。

## 影响

- Claude CLI 安装流程与官方保持一致，避免额外路径处理带来的潜在问题。

## 回退方案

- 恢复原有的 CLI 安装与拷贝逻辑。
