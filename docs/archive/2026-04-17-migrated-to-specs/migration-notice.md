# 文档迁移说明 / Migration Notice

> 本页面已归档，仅作历史记录。

---

## 迁移概述 / Migration Overview

**日期 / Date**: 2026-04-17

本次迁移将项目规范文档从 `docs/` 目录迁移到 `/specs/` 目录，遵循 **Spec-Driven Development (SDD)** 范式。

This migration moved specification documents from `docs/` to `/specs/`, following the **Spec-Driven Development (SDD)** paradigm.

---

## 迁移内容 / Migrated Content

| 原路径 / Original Path | 新路径 / New Path |
|------------------------|-------------------|
| `docs/steering/` | `specs/product/` |
| `docs/benchmark-system/` | `specs/rfc/` |
| `docs/memory-pool/` | `specs/rfc/` |
| `docs/advanced-cpp-tooling/` | `specs/rfc/` |

---

## 新目录结构 / New Directory Structure

```
specs/
├── product/       # 产品功能定义与验收标准
├── rfc/           # 技术设计文档
├── api/           # API 接口规范
├── db/            # 数据模型与配置规范
└── testing/       # 测试策略与约定
```

---

## 参考链接 / Reference Links

- [specs/README.md](../../../specs/README.md) — 规范文档目录索引
- [AGENTS.md](../../../AGENTS.md) — AI Agent 工作流指南
