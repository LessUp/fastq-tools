# Changelog 目录

本目录存放细粒度的变更记录，每次有意义的变更对应一个独立的 Markdown 文件。

主 CHANGELOG 见根目录 [`CHANGELOG.md`](../CHANGELOG.md)（按版本号组织）。

## 命名规范

```
YYYY-MM-DD-<slug>.md
```

- **日期**：变更发生的日期，格式 `YYYY-MM-DD`
- **slug**：简短描述，使用全小写 **kebab-case**（如 `devcontainer-proxy-fix`）
- 禁止使用大写、PascalCase 或 SCREAMING_CASE

### 示例

```
2026-01-15-io-and-stat-fixes.md        ✅
2026-01-15-IO-FIXES.md                 ❌ 大写
2026-01-15-IoFixes.md                  ❌ PascalCase
```

## 内容格式

推荐使用以下结构：

```markdown
# YYYY-MM-DD 简短标题

## 背景
（为什么要做这个变更）

## 变更
（具体改了什么）

## 影响
（对使用者/开发者的影响）

## 回退方案
（可选，如何回退此变更）
```

## 合并原则

- **同一天、同一主题** 的多个小变更应合并为一个文件，避免过度碎片化
- 合并后的文件在标题下方加注：`> 本文件合并自 N 个同日同主题的独立 changelog 条目。`
- 不同主题的变更即使在同一天也应保持独立文件
