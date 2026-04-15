# 2026-04-16 文档全面优化

## 概述

对 FastQTools 项目文档进行全面优化和重构，提升文档质量和可维护性。

## 变更详情

### README 优化

- 精简 `README.md` 和 `README.zh-CN.md`，突出核心功能和快速开始
- 更新技术特性描述，确保与当前实现一致
- 简化项目结构说明

### CHANGELOG 重构

- 将 `CHANGELOG.md` 简化为版本摘要
- 将 75 个历史 changelog 文件归档到 `changelog/archive/` 目录
- 更新 `changelog/README.md`，明确归档规则

### 文档目录优化

- 精简 `docs/index.md`，提升首页可读性
- 更新 `docs/README.md`，简化目录结构说明
- 保留 `docs/guide/`、`docs/dev/`、`docs/api/` 等核心文档

## 影响范围

- 文档结构更清晰，便于维护
- 历史变更归档，减少主目录文件数量
- 无代码变更，不影响功能

## 相关文件

- `README.md`
- `README.zh-CN.md`
- `CHANGELOG.md`
- `docs/index.md`
- `docs/README.md`
- `changelog/README.md`
- `changelog/archive/` 目录结构
