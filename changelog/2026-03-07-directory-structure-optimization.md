# 项目目录结构优化

**日期**: 2026-03-07
**类型**: chore
**范围**: project

## 变更内容

### 清理无用目录
- 移除 `.kiro/`（空的 Kiro AI 工具配置）
- 移除 `.qoder/`（Qoder AI 工具自动生成内容）
- 移除 `.spec-workflow/`（空的 spec workflow 模板）
- 移除 `cascade/`（Cascade 对话历史记录，非项目代码）
- 移除 `_output/`（空目录，用途不明）

### 整理根目录文档
- `DEPLOYMENT.md` → `docs/deployment.md`
- `HELP.md` → `docs/dev/docker-ssh-help.md`
- `MIGRATION.md` → `docs/migration.md`

### 整合 Docker 文件
- `config/deployment/Dockerfile` → `docker/Dockerfile.deploy`
- 所有 Docker 相关文件现统一存放在 `docker/` 目录

### 优化 .gitignore
- 去除重复条目（`*.gcda`、`*.gcno`、`*.gcov` 重复定义）
- 合并冗余构建目录规则
- 添加 AI 工具目录忽略规则（`.kiro/`、`.qoder/`、`.spec-workflow/`、`cascade/`）
- 添加 `_output/` 忽略规则
- 按功能分类整理，添加分区标题

### 更新 .dockerignore
- 添加 AI 工具目录忽略规则

### 更新文档引用
- 更新 `AGENTS.md` 目录结构树
- 更新 `README.md` 项目结构说明
- 更新 `config/README.md` 移除已迁移的 deployment 条目
- 更新 `scripts/core/README.md` 中 MIGRATION.md 引用路径

## 根目录文件清单（优化后）

保留在根目录的标准文件（GitHub 识别）：
- `README.md`、`LICENSE`、`CHANGELOG.md`
- `CODE_OF_CONDUCT.md`、`CONTRIBUTING.md`、`SECURITY.md`
- `AGENTS.md`（AI 工具标准位置）
- `.clang-format`、`.clang-tidy`、`.editorconfig`
- `CMakeLists.txt`、`CMakePresets.json`、`conanfile.py`
- `Doxyfile`、`commitlint.config.js`
- `.gitignore`、`.gitattributes`、`.git-blame-ignore-revs`、`.gitmessage.txt`、`.dockerignore`
