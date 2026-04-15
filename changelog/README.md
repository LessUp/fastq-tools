# Changelog 目录

本目录管理 FastQTools 项目的变更记录。

---

## 目录结构

```
changelog/
├── README.md                    # 本文件
├── 2025-summary.md              # 2025 年度汇总
├── 2026-summary.md              # 2026 年度汇总
├── YYYY-MM-DD-<slug>.md         # 当前月份的变更记录
└── archive/                     # 历史变更归档
    ├── 2025/                    # 2025 年详细变更（50 个文件）
    └── 2026/                    # 2026 年详细变更（26 个文件）
```

---

## 使用指南

### 查看变更

- **年度汇总**：阅读 `YYYY-summary.md` 了解年度主要变更
- **详细记录**：查看 `archive/YYYY/` 目录中的具体文件
- **最新变更**：根目录的 `YYYY-MM-DD-*.md` 文件

### 主 CHANGELOG

根目录 [`CHANGELOG.md`](../CHANGELOG.md) 按版本号组织，用于发布说明。

---

## 编写规范

### 文件命名

```
YYYY-MM-DD-<slug>.md
```

- **日期**：变更日期，格式 `YYYY-MM-DD`
- **slug**：简短描述，kebab-case（全小写，连字符分隔）

### 示例

```
2026-04-16-code-quality-fixes.md    ✅ 正确
2026-04-16-CodeQualityFixes.md      ❌ PascalCase
2026-04-16-CODE-QUALITY-FIXES.md    ❌ SCREAMING_CASE
```

### 内容模板

```markdown
# YYYY-MM-DD 简短标题

## 概述
（变更的简要描述）

## 变更详情
（具体改了什么）

## 影响范围
（对使用者/开发者的影响）
```

---

## 归档规则

1. **当前月份**的变更保留在根目录
2. **每月月初**将上月变更移入 `archive/YYYY/`
3. **年度汇总**在每年年初更新

---

## 变更统计

| 年份 | 变更数量 | 主要主题 |
|------|----------|----------|
| 2025 | 50 | 架构设计、核心功能、命名规范 |
| 2026 | 28 | 工具链升级、DevContainer、性能优化 |
