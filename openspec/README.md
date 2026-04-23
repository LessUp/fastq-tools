# OpenSpec Specifications

本目录包含 FastQTools 项目的所有规范文档，遵循 **OpenSpec 规范驱动开发 (SDD)** 框架。

> **所有代码实现必须以这些规范为唯一事实来源 (Single Source of Truth)。**

## 目录结构

```
openspec/
├── manifest.yaml              # 项目配置清单
├── baseline/                  # 基础规范
│   ├── product/               # 产品需求定义
│   ├── architecture/          # 架构决策记录
│   ├── api/                   # 公共 API 接口规范
│   ├── schemas/               # 数据模型与配置模式
│   └── testing/               # 测试策略与约定
├── changes/                   # 活跃变更提案
├── archive/                   # 已完成变更归档
└── templates/                 # 文档模板
```

## 规范类型

### Product Specs (`baseline/product/`)

- **用途**: 定义产品功能、用户故事和验收标准
- **读者**: 产品经理、开发者、用户
- **格式**: Markdown，包含用户故事和验收标准
- **示例**: `baseline/product/fastq-processing.md`

### Architecture Specs (`baseline/architecture/`)

- **用途**: 技术设计文档、架构决策、实现策略
- **读者**: 开发者、架构师
- **格式**: 编号文件 (`NNNN-slug.md`)，Context → Decision → Consequences 结构
- **示例**: `baseline/architecture/0001-core-architecture.md`

### API Specs (`baseline/api/`)

- **用途**: 公共接口定义、函数签名、数据类型
- **读者**: 开发者、库消费者
- **格式**: C++ 代码示例与接口定义
- **示例**: `baseline/api/core-api.md`

### Schema Specs (`baseline/schemas/`)

- **用途**: 配置格式、数据模型、输出模式
- **读者**: 开发者、集成工程师
- **格式**: JSON Schema、字段定义、验证规则
- **示例**: `baseline/schemas/schema.md`

### Testing Specs (`baseline/testing/`)

- **用途**: 测试策略、约定、覆盖率要求
- **读者**: 开发者、QA 工程师
- **格式**: 测试组织、框架、示例
- **示例**: `baseline/testing/test-strategy.md`

## OpenSpec 工作流

### 维护期判定

- `baseline/` 仍然是当前实现的唯一事实来源。
- 只有当改动影响**行为、公共 API、schema / 文件格式、架构 / 工具链 / 发布策略、兼容性或弃用策略**时，才需要进入 `openspec/changes/` proposal 流程。
- docs、测试、注释、实现内整理等**不改变 baseline 含义**的改动，可以直接实施并推送，但提交说明应注明相关 baseline 引用或 `no baseline delta`。

### 1. 创建提案（仅在跨越 proposal 阈值时）

在 `openspec/changes/<name>/` 中维护最小化提案：

```
openspec/changes/<name>/
├── proposal.md    # Why, What Changes, Impact
├── specs/         # 新增/修改的规范
├── design.md      # 技术方案
└── tasks.md       # 实现任务清单
```

维护冻结前的提案尽量轻量：

- `proposal.md`：说明 Why、影响的 baseline、兼容性 / 风险、验证方式。
- `design.md`：仅在架构、工具链、性能风险较高时需要。
- `tasks.md`：围绕最小可交付改动切分，不做长期 roadmap。

### 2. 执行实现

按当前工作分支执行任务清单；若风险较高，可自行增加 branch / worktree 隔离：

- 按 `tasks.md` 顺序完成任务
- 100% 遵守规范定义
- 不添加规范外功能
- 提交说明应回链 proposal / baseline，并记录实际验证结果

### 3. 归档变更

仅当对应改动已经稳定落地（已提交/已推送）后，才归档：

- 移动至 `openspec/archive/YYYY/MM-<name>/`
- 更新 `baseline/` 规范
- 在归档中回填 commit SHA（如适用）
- 尚未落地的提案继续保留在 `openspec/changes/`

## 模板

| 模板 | 用途 |
|------|------|
| `templates/proposal.md` | 变更提案 |
| `templates/spec.md` | 规范文档 |
| `templates/design.md` | 技术设计 |
| `templates/tasks.md` | 任务清单 |

## 与历史 specs/ 目录的映射

| 原 specs/ 目录 | 新 OpenSpec 目录 | 说明 |
|---------------|-----------------|------|
| `specs/product/` | `baseline/product/` | 产品需求 |
| `specs/rfc/` | `baseline/architecture/` | RFC → 架构决策记录 |
| `specs/api/` | `baseline/api/` | API 规范 |
| `specs/db/` | `baseline/schemas/` | 数据模型 |
| `specs/testing/` | `baseline/testing/` | 测试策略 |

## 相关文档

- [AGENTS.md](../AGENTS.md) - AI Agent 工作流规范
- [CLAUDE.md](../CLAUDE.md) - Claude Code 指南
- [CONTRIBUTING.md](../CONTRIBUTING.md) - 贡献指南
- [OpenSpec GitHub](https://github.com/Fission-AI/OpenSpec) - OpenSpec 框架
