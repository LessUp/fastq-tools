# 2026-03-07 文档目录彻底重构

## 类型

refactor(docs)

## 变更内容

彻底重构 `docs/` 目录，消除大量重复内容，建立清晰的文档分层体系。

### 新增

- `docs/guide/` — 用户指南目录
  - `getting-started.md` — 快速开始（合并自 installation.md + wiki/快速开始 + wiki/安装指南）
  - `cli-reference.md` — CLI 完整参考（合并自 guide/usage.md + wiki/命令行使用指南）
  - `configuration.md` — 配置管理（合并自 wiki/配置管理）
  - `deployment.md` — 部署指南（合并自 deployment.md + wiki/部署）
- `docs/api/` — API 参考目录
  - `overview.md` — API 概览与快速示例
  - `io.md` — IO 模块 API（FastqReader/Writer/Record/Batch）
  - `processing.md` — 处理流水线 API（Pipeline/Predicate/Mutator）
  - `statistics.md` — 统计分析 API
  - `core.md` — 核心工具 API（Config/Error/Logging）
- `docs/dev/testing.md` — 测试策略（合并自 wiki/开发指南/测试策略）
- `docs/dev/devcontainer.md` — DevContainer 统一文档（合并自 devcontainer-guidelines + devcontainer-ssh + docker-ssh-help 三个文件）

### 移动

- `docs/migration.md` → `docs/dev/migration.md`

### 删除

- `docs/wiki/` — 整个目录（自动生成内容，已合并到 guide/、api/、dev/）
- `docs/deployment.md` — 已合并到 guide/deployment.md
- `docs/installation.md` — 已合并到 guide/getting-started.md
- `docs/guide/usage.md` — 已合并到 guide/cli-reference.md
- `docs/dev/devcontainer-guidelines.md` — 已合并到 dev/devcontainer.md
- `docs/dev/devcontainer-ssh.md` — 已合并到 dev/devcontainer.md
- `docs/dev/docker-ssh-help.md` — 已合并到 dev/devcontainer.md

### 更新

- `docs/README.md` — 重写文档索引，反映新目录结构
- `docs/dev/build.md` — 更新所有过时脚本引用（build.sh → scripts/core/build 等）

## 动机

旧结构存在严重问题：
1. wiki/、dev/、guide/ 之间大量内容重叠
2. deployment.md、installation.md 散落在根目录
3. 三个 devcontainer 相关文件内容分散
4. build.md 中引用的脚本路径已过时
5. 缺少独立的 API 参考和测试策略文档
