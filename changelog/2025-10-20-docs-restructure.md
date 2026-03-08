# 2025-10-20 文档清理与重组

> 本文件合并自 2 个同日同主题的独立 changelog 条目。

## 背景

`docs/` 目录存在重复文档、过时内容和碎片化结构，需要清理合并并建立三层文档体系。

## 变更

### 1. 文档清理与合并

- 编辑 `docs/user/usage.md`、`quick_start.md`、`installation.md`：移除过时参数、统一系统要求
- 编辑 `docs/dev/architecture.md`、`coding_standards.md`：统一为 C++20
- 新增 `docs/dev/build.md`：合并 `build_system.md` 与 `building.md`
- 新增 `docs/internal/annotation_tasks.md`：合并内部注释任务清单
- 删除重复/过时文档：`build_system.md`、`building.md`、`tbb_pipeline.md`、`commenting_standards.md` 等

### 2. 文档重组

- 建立 User/Dev/Internal 三层结构
- 新增 `docs/README.md` 文档索引
- 删除 `src/common/`（未被引用，已由 `src/modules/common/` 替代）

## 影响

- 不影响构建/测试/CI
- 文档导航更清晰，面向教学与开源使用者
