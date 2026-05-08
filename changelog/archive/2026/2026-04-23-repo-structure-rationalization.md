# 2026-04-23 仓库结构与文档资产收敛

## 概述 / Overview

针对仓库中明显可再生、低价值或过时的文档资产进行收敛清理，降低维护噪音，同时保持公开文档入口稳定。

## 变更详情 / Details

### 移除的资产

- 删除已提交的 MkDocs 站点输出目录 `site/`
- 删除已提交的基准测试生成产物：
  - `docs/performance/benchmark-reports/*.json`
  - `docs/performance/benchmark-reports/latest.md`
- 删除一次性迁移文档：
  - `docs/dev/migration.md`
  - `docs/dev/migration.en.md`
  - `docs/migration/openspec-migration.md`
- 删除与站点首页重复且会触发 MkDocs 严格构建警告的 `docs/README.md`
- 删除低价值审计草稿：`docs/archive/2025-12-29-audit/thinking-process.md`

### 收敛与修正

- 更新 `.gitignore`，避免再次提交基准测试生成报告
- 将 README 中的性能报告链接统一到稳定页面 `docs/performance/benchmark-report.md`
- 简化归档入口与迁移说明，明确归档资料仅供历史查询
- 更新 `changelog/2026-04-23-openspec-integration.md`，移除对已删除迁移文档的引用

## 影响范围 / Impact

- 仓库根目录更接近“源码 + 必要文档”的低维护结构
- 公开文档链接继续有效，且优先指向稳定入口
- 生成型 benchmark 资产改为按需生成，而非长期占用版本库

## 验证 / Verification

- [x] `python3 -m mkdocs build --strict --site-dir build/docs-site-validation`
- [x] `rg "](docs/migration/openspec-migration.md|](docs/dev/migration|performance/benchmark-reports/latest.md" README.md README.zh-CN.md docs`
