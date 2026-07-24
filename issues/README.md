# 问题复盘

轻量本地记录，用于问题追踪、根因分析和经验沉淀。每个问题一个 markdown 文件，git 管理状态变更。

## 什么时候写

- 遇到并解决了一个非显而易见的问题
- 做了一个需要记录理由的技术决策
- 发现了一个待处理的改进点
- 复盘一次性能优化或架构调整

不需要写的：显而易见的 bug 修复、日常小改动。

## 格式

文件名：`NNN-<kebab-case-slug>.md`，NNN 为三位递增编号。

```markdown
---
id: NNN
title: 一句话标题
status: open | closed
labels: [bug, refactor, performance, docs, decision]
created: YYYY-MM-DD
closed: YYYY-MM-DD 或留空
---

## 现象

遇到了什么，或者想改进什么。

## 根因

为什么会出现这个问题。（关闭时填写）

## 处理

怎么解决的，或者计划怎么处理。

## 复盘

经验教训：下次怎么避免？有什么可以复用的模式？（关闭时填写）
```

字段说明：
- `status` 只有 open / closed，不需要 in-progress——做了就关闭，没做就开着
- `labels` 可多选，用于 grep 过滤
- "根因"和"复盘"在关闭时填写，是复盘的核心产出

## 检索

```bash
grep -l "status: open" issues/*.md          # 待处理
grep -l "labels:.*performance" issues/*.md  # 按标签
grep -rl "根因" issues/*.md                  # 有根因分析的
```

## 索引

| # | 标题 | 状态 | 标签 | 创建 |
|---|------|------|------|------|
| — | （暂无） | — | — | — |

> 新增/关闭时更新此表。