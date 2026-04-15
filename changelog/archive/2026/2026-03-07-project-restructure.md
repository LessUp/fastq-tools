# 2026-03-07 项目目录全面重构

> 本文件合并自 4 个同日同主题的独立 changelog 条目。

## 背景

项目积累了大量无用目录、重复文档、过时路径引用，需要全面重构目录结构。

## 变更

### 1. 项目目录结构优化

- 清理无用目录：`.kiro/`、`.qoder/`、`.spec-workflow/`、`cascade/`、`_output/`
- 根目录文档整理：`DEPLOYMENT.md` → `docs/deployment.md`、`HELP.md` → `docs/dev/docker-ssh-help.md`、`MIGRATION.md` → `docs/migration.md`
- Docker 文件统一：`config/deployment/Dockerfile` → `docker/Dockerfile.deploy`
- 迁移有价值文档：`.kiro/steering/` → `docs/specs/steering/`、`.kiro/specs/` → `docs/specs/`、`.qoder/repowiki/` → `docs/wiki/`
- 优化 `.gitignore` 和 `.dockerignore`

### 2. 文档目录彻底重构

- 新增 `docs/guide/`：getting-started、cli-reference、configuration、deployment（合并重复内容）
- 新增 `docs/api/`：overview、io、processing、statistics、core（独立 API 参考）
- 新增 `docs/dev/testing.md`、`docs/dev/devcontainer.md`（合并 3 个 devcontainer 文件）
- 删除 `docs/wiki/`（已合并到 guide/api/dev）、`docs/installation.md`、`docs/deployment.md` 等重复文件

### 3. tests 目录重构

- `TestHelpers` → `TestDataGenerator`，移除与 `FixtureLoader` 重复的方法
- `FastQToolsTest` 基类改用 RAII 风格 `TempDirectory`
- 修正 `testDataDir_` 路径：`tests/fixtures/` → `tools/data/`
- CMakeLists.txt 清理冗余 `find_package`、硬编码 include 路径
- `tests/README.md` 从 430 行精简到 ~165 行

### 4. tools 目录重构

- `tools/benchmark/` 新增 `scripts/` 子目录，分离 Python/Shell 脚本与 C++ 源码
- 新增 `tools/benchmark/README.md`、`tools/data/README.md`、`tools/fuzz/README.md`
- 更新 `.github/workflows/benchmark.yml` 等引用路径

## 影响

- 文档层次清晰：guide（用户）→ api（参考）→ dev（开发）→ specs（设计）→ archive（归档）
- 测试工具库代码量减少，路径引用修正
- 不影响核心功能代码
