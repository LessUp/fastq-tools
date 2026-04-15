# 2026-03-08 Changelog 目录彻底整理重构

## 背景

`changelog/` 目录积累了 102 个文件，存在以下问题：
- 同日同主题过度碎片化（如 2025-03-08 有 4 个独立 CI 修复文件、2026-03-08 有 17 个文件）
- 格式不一致：部分文件使用英文、部分标题缺少日期前缀
- 内容质量参差不齐：部分文件是设计思考日志而非变更记录（如 optimization-libdeflate），部分过度冗长（installation-fixes 达 6KB 英文）

## 变更

### 1. 合并同日同主题碎片（55 个旧文件 → 16 个合并文件）

| 合并后文件 | 原始文件数 | 主题 |
|-----------|-----------|------|
| `2025-03-08-ci-fixes.md` | 4 | CI 工作流修复 |
| `2025-10-20-repo-audit-and-restructure.md` | 3 | 仓库审计与重构 |
| `2025-10-20-scripts-cleanup.md` | 2 | 脚本精简 |
| `2025-10-20-docs-restructure.md` | 2 | 文档重组 |
| `2025-10-21-architecture-refactor.md` | 4 | 架构重构 Phase 1-2 |
| `2025-10-21-rename-overhaul.md` | 2 | 命名重构批次 1-4 |
| `2025-12-24-devcontainer-setup.md` | 2 | DevContainer 配置 |
| `2025-12-24-windsurf-workflows.md` | 3 | Windsurf Workflow |
| `2025-12-30-performance-optimizations.md` | 3 | 性能优化设计 |
| `2026-01-08-devcontainer-fixes.md` | 4 | DevContainer 修复 |
| `2026-01-08-scripts-refactor.md` | 2 | 脚本系统重构 |
| `2026-01-23-devcontainer-and-docker.md` | 3 | DevContainer 与 Docker |
| `2026-03-07-project-restructure.md` | 4 | 项目目录重构 |
| `2026-03-08-project-cleanup.md` | 7 | 项目目录清理 |
| `2026-03-08-build-fixes.md` | 7 | 构建路径与代码修复 |
| `2026-03-08-toolchain-upgrade.md` | 3 | 工具链版本升级 |

### 2. 重写质量差的独立文件

- `2025-10-23-dependency-cleanup.md`：英文 → 中文，补充标准格式
- `2025-12-30-installation-fixes.md`：6KB 英文冗长内容 → 精简中文（30 行）

### 3. 保留格式良好的独立文件

已合并过的文件（如 `2025-12-25-naming-convention-refactor.md`）和格式规范的独立文件保持不变。

## 影响

- 文件数量：102 → 64（减少 37%）
- 所有原始内容的核心信息完整保留在合并文件中
- 合并文件统一使用 `> 本文件合并自 N 个同日同主题的独立 changelog 条目。` 标注
- 格式统一：中文、日期前缀标题、背景/变更/影响结构
