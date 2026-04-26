# Changelog Directory / 变更记录目录

This directory manages change records for the FastQTools project.
本目录管理 FastQTools 项目的变更记录。

---

## Directory Structure / 目录结构

```
changelog/
├── README.md                    # This file / 本文件
├── TEMPLATE.md                  # Standard template / 标准模板
├── CHANGELOG.md                 # Main changelog (root) / 主变更日志（根目录）
├── 2025-summary.md              # 2025 Annual summary / 2025 年度汇总
├── 2026-summary.md              # 2026 Annual summary / 2026 年度汇总
├── YYYY-MM-DD-<slug>.md         # Current month changes / 当前月份变更
└── archive/                     # Historical archive / 历史归档
    ├── 2025/                    # 2025 detailed changes (50 files) / 2025 详细变更
    └── 2026/                    # 2026 detailed changes / 2026 详细变更
```

---

## Usage Guide / 使用指南

### Viewing Changes / 查看变更

- **Annual Summary / 年度汇总**: Read `YYYY-summary.md` for annual major changes
- **Detailed Records / 详细记录**: View files in `archive/YYYY/` directory
- **Latest Changes / 最新变更**: Files `YYYY-MM-DD-*.md` in root directory

### Main Changelog / 主变更日志

Root directory [`CHANGELOG.md`](../CHANGELOG.md) organized by version number, used for release notes.
根目录 [`CHANGELOG.md`](../CHANGELOG.md) 按版本号组织，用于发布说明。

---

## Writing Standards / 编写规范

### File Naming / 文件命名

```
YYYY-MM-DD-<slug>.md
```

- **Date / 日期**: Change date, format `YYYY-MM-DD`
- **slug**: Brief description, kebab-case (lowercase, hyphen-separated)

### Examples / 示例

```
2026-04-16-code-quality-fixes.md    ✅ Correct / 正确
2026-04-16-CodeQualityFixes.md      ❌ PascalCase
2026-04-16-CODE-QUALITY-FIXES.md    ❌ SCREAMING_CASE
```

### Content Template / 内容模板

Use [`TEMPLATE.md`](TEMPLATE.md) as the standard format.
使用 [`TEMPLATE.md`](TEMPLATE.md) 作为标准格式。

```markdown
# YYYY-MM-DD Change Title / 简短标题

## Overview / 概述
(Brief description of change / 变更的简要描述)

## Details / 变更详情
(Specific changes made / 具体改了什么)

## Impact / 影响范围
(Impact on users/developers / 对使用者/开发者的影响)

## Verification / 验证
- [ ] Tests pass / 测试通过
- [ ] Documentation updated / 文档已更新
```

---

## Archiving Rules / 归档规则

1. **Current month** changes are kept in root directory
2. **Beginning of each month**: Move previous month's changes to `archive/YYYY/`
3. **Annual summary**: Update at the beginning of each year

---

## Change Statistics / 变更统计

| Year / 年份 | Archive Count / 归档数 | Active / 活跃条目 | Main Topics / 主要主题 |
|-------------|---|---|---|
| 2025 | 50 | 1 summary | Architecture design, core functionality, naming conventions / 架构设计、核心功能、命名规范 |
| 2026 (Apr) | 26 (archiving) | 8 current | Phase 1 baseline alignment, docs cleanup, toolchain precision / Phase 1 真值对齐、文档清理、工具链精化 |

**当前策略**：仅保留当前月份（2026-04-）的活跃条目在根目录，历史改动自动归档到 `archive/YYYY/`。

---

## Quick Links / 快速链接

- [Template / 模板](TEMPLATE.md)
- [Main Changelog / 主变更日志](../CHANGELOG.md)
- [2025 Summary / 2025 汇总](2025-summary.md)
- [2026 Summary / 2026 汇总](2026-summary.md)
