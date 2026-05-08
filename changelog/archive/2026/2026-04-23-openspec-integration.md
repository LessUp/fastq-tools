# 2026-04-23 整合 OpenSpec 框架

## 概述 / Overview

将项目规范目录从 `specs/` 迁移到 `openspec/`，整合 OpenSpec 框架的规范驱动开发 (SDD) 工作流。

## 变更类型 / Change Type

`refactor(docs)`

## 变更详情 / Details

### 新增

1. **openspec/ 目录结构**
   - `manifest.yaml` - OpenSpec 配置清单
   - `baseline/` - 基础规范目录（原 specs/ 内容）
   - `changes/` - 活跃变更提案目录
   - `archive/` - 已完成变更归档
   - `templates/` - 标准化文档模板

2. **文档模板**
   - `templates/proposal.md` - 变更提案模板
   - `templates/spec.md` - 规范文档模板
   - `templates/design.md` - 技术设计模板
   - `templates/tasks.md` - 任务清单模板

3. **目录索引**
   - `openspec/README.md`

### 迁移

| 原路径 | 新路径 |
|-------|--------|
| `specs/product/` | `openspec/baseline/product/` |
| `specs/rfc/` | `openspec/baseline/architecture/` |
| `specs/api/` | `openspec/baseline/api/` |
| `specs/db/` | `openspec/baseline/schemas/` |
| `specs/testing/` | `openspec/baseline/testing/` |

### 修改

- `AGENTS.md` - 更新 SDD 工作流为 OpenSpec 工作流（5 步流程）
- `CLAUDE.md` - 更新目录结构说明和所有引用路径
- `CONTRIBUTING.md` - 更新规范相关引用路径

### 删除

- `specs/` 整个目录（已迁移到 openspec/baseline/）

## 影响范围 / Impact

### 目录结构变更

- **原**: `specs/{product,rfc,api,db,testing}/`
- **新**: `openspec/baseline/{product,architecture,api,schemas,testing}/`

### 工作流变更

| 原步骤 | 新步骤 |
|-------|--------|
| Step 1: 审查规范 | Step 1: 审查规范 (baseline) |
| Step 2: 规范优先 | Step 2: 创建提案 (`/opsx:propose`) |
| Step 3: 代码实现 | Step 3: 执行实现 (`/opsx:apply`) |
| Step 4: 测试验证 | Step 4: 测试验证 |
| - | Step 5: 归档变更 (`/opsx:archive`) |

### 引用路径更新

所有 `/specs/` 引用已更新为 `/openspec/baseline/`

## 验证 / Verification

```bash
# 验证目录结构
ls -la openspec/baseline/

# 验证旧目录已删除
ls specs/  # 应不存在

# 验证构建
./scripts/core/build
./scripts/core/test
```

## 相关链接 / Related Links

- [OpenSpec GitHub](https://github.com/Fission-AI/OpenSpec)
- [openspec/README.md](../openspec/README.md)
- [OpenSpec 索引](../openspec/README.md)

## 备注 / Notes

- OpenSpec CLI 需要安装: `npm install -g @fission-ai/openspec@latest`
- 后续开发使用 `/opsx:propose`、`/opsx:apply`、`/opsx:archive` 命令管理变更
