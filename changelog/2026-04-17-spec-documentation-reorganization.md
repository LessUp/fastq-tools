# 文档结构重组：Spec-Driven Development 规范化

> **日期**: 2026-04-17  
> **类型**: docs  
> **影响范围**: 项目文档结构、AI Agent 规则文件

## 目标

按照 Spec-Driven Development (SDD) 最佳实践，对项目文档结构进行彻底整理，确保规范文档与常规文档清晰分离。

## 主要改动

### 1. AGENTS.md 重构

- 按照 SDD 模板重新组织文档结构
- 明确声明 SDD 工作流的四个步骤
- 添加 Directory Context 和 Code Generation Rules 章节
- 保留项目特定的构建、测试、代码风格指南

### 2. CLAUDE.md 更新

- 与 AGENTS.md 保持 SDD 工作流声明一致
- 修正工具链规范链接指向 `specs/rfc/0002-toolchain-policy.md`
- 添加 Related Documents 章节链接相关文档

### 3. 目录清理

- 删除已迁移的空目录：`docs/decisions/`、`docs/specs/`
- `specs/` 目录结构已符合规范：
  - `specs/product/` — 产品需求规范
  - `specs/rfc/` — 技术设计文档 (RFC)
  - `specs/api/` — API 接口规范
  - `specs/db/` — 数据模型规范
  - `specs/testing/` — 测试策略规范

### 4. README 文件修正

- 确认 `README.md` 为英文默认版本
- 修正 `README.zh-CN.md` 语言切换链接顺序
- 移除 `README.zh-CN.md` 中重复的文字

## 影响范围

| 文件 | 变更类型 |
|------|---------|
| `AGENTS.md` | 重写 |
| `CLAUDE.md` | 更新 |
| `README.zh-CN.md` | 修正 |
| `docs/decisions/` | 删除空目录 |
| `docs/specs/` | 删除空目录 |

## 验证方式

```bash
# 验证目录结构
ls -la specs/
ls -la docs/

# 验证文档链接
grep -r "specs/rfc" AGENTS.md CLAUDE.md README.md

# 验证空目录已删除
test ! -d docs/decisions && echo "docs/decisions 已删除"
test ! -d docs/specs && echo "docs/specs 已删除"
```

## 相关文档

- [AGENTS.md](../AGENTS.md) — AI Agent 规则文件
- [CLAUDE.md](../CLAUDE.md) — Claude Code 指南
- [specs/README.md](../specs/README.md) — 规范文档目录索引
- [docs/archive/migration-notice-2026-04-17.md](../docs/archive/migration-notice-2026-04-17.md) — 迁移通知
