# FastQTools 文档中心

> 本目录包含 FastQTools 项目的所有文档资源。

---

## 📁 目录结构

```
docs/
├── README.md                          # 本文件 - 文档索引
├── installation.md                    # 安装指南
│
├── guide/                             # 用户指南
│   └── usage.md                       # CLI 使用指南
│
├── dev/                               # 开发者文档（活跃维护）
│   ├── architecture.md                # 架构设计
│   ├── design.md                      # 核心设计
│   ├── build.md                       # 构建指南
│   ├── coding-standards.md            # 编码规范
│   ├── git-guidelines.md              # Git 提交规范与工作流
│   ├── quality-tools.md               # 代码质量工具指南
│   ├── benchmark-guide.md             # Benchmark 使用与报告指南
│   ├── devcontainer-guidelines.md     # Dev Container 团队规范
│   └── devcontainer-ssh.md            # Windsurf/Cursor SSH 方案
│
├── decisions/                         # 架构决策记录（ADR）
│   ├── optimization-decisions.md      # 优化决策与辩证分析
│   └── project-assessment.md          # 项目评估报告
│
├── benchmark-reports/                 # 基准测试数据（自动生成）
│   ├── results/                       # 历史测试结果
│   ├── latest.json                    # 最新结果（JSON）
│   └── latest.md                      # 最新结果（Markdown）
│
└── archive/                           # 归档（历史性、一次性文档）
    ├── audit-2025-12-29-docs-style.md # 文档风格审计
    ├── thinking-process.md            # 分析思维过程
    ├── implementation-progress.md     # 实现进度快照
    └── requirements.md                # 需求文档快照
```

---

## 📖 快速导航

### 新用户入门

1. [安装指南](installation.md) - 环境配置和安装步骤
2. [使用指南](guide/usage.md) - 命令行用法和示例

### 开发者指南

1. [架构设计](dev/architecture.md) - 系统架构和设计原则
2. [核心设计](dev/design.md) - 数据模型、流水线、日志等
3. [编码规范](dev/coding-standards.md) - 代码风格和命名约定
4. [构建指南](dev/build.md) - 构建系统和依赖管理
5. [Git 工作流](dev/git-guidelines.md) - 提交规范和分支策略
6. [代码质量工具](dev/quality-tools.md) - Sanitizers、Valgrind、静态分析、覆盖率、模糊测试
7. [Benchmark 指南](dev/benchmark-guide.md) - 性能基准测试使用与报告
8. [Dev Container 规范](dev/devcontainer-guidelines.md) - Windows + WSL2 + Docker Desktop
9. [DevContainer SSH](dev/devcontainer-ssh.md) - Windsurf/Cursor 远程 SSH 方案

### 架构决策

1. [优化决策](decisions/optimization-decisions.md) - 优化需求、辩证分析、实施状态
2. [项目评估](decisions/project-assessment.md) - 架构、性能、依赖、测试全面评估

---

## 📝 文档维护

### 命名约定

- 文档文件: `kebab-case.md`
- 目录名: `lowercase`

### 更新记录

| 日期 | 更新内容 |
|------|----------|
| 2025-02-24 | 重构文档目录：合并重复内容、消除碎片目录、引入 decisions/ 和 archive/ |
| 2025-12-30 | 添加项目分析文档，重组目录结构 |
| 2025-12-29 | 文档风格统一 |

---

*如有问题，请提交 Issue 或 PR。*
