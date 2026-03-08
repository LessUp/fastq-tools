# FastQTools 文档中心

> 本目录包含 FastQTools 项目的所有文档资源，按受众和用途组织。

---

## 目录结构

```
docs/
├── README.md                          # 本文件 — 文档索引
│
├── guide/                             # 用户指南（面向使用者）
│   ├── getting-started.md             # 快速开始（安装 + 首次使用）
│   ├── cli-reference.md               # CLI 完整参考
│   ├── configuration.md               # 配置管理
│   └── deployment.md                  # 部署指南（Docker + 生产环境）
│
├── dev/                               # 开发者文档（面向开发者）
│   ├── architecture.md                # 架构设计
│   ├── design.md                      # 核心设计（数据模型、流水线）
│   ├── build.md                       # 构建指南
│   ├── testing.md                     # 测试策略
│   ├── coding-standards.md            # 编码规范
│   ├── git-guidelines.md              # Git 提交规范与工作流
│   ├── quality-tools.md               # 代码质量工具
│   ├── benchmark-guide.md             # Benchmark 使用与报告
│   ├── devcontainer.md                # DevContainer 开发环境
│   └── migration.md                   # 脚本迁移指南
│
├── api/                               # API 参考（面向库集成）
│   ├── overview.md                    # API 概览与快速示例
│   ├── io.md                          # IO 模块（Reader/Writer/Record/Batch）
│   ├── processing.md                  # 处理流水线（Pipeline/Predicate/Mutator）
│   ├── statistics.md                  # 统计分析
│   └── core.md                        # 核心工具（Config/Error/Logging）
│
├── decisions/                         # 架构决策记录（ADR）
│   ├── toolchain-policy.md            # 工具链版本规范（编译器、Docker、CMake）
│   ├── optimization-decisions.md      # 优化决策与辩证分析
│   └── project-assessment.md          # 项目评估报告
│
├── specs/                             # 功能规格
│   ├── steering/                      # 产品/技术/结构规格
│   ├── advanced-cpp-tooling/          # 高级 C++ 工具链规格
│   ├── benchmark-system/              # 基准测试系统规格
│   └── memory-pool/                   # 内存池规格
│
├── benchmark-reports/                 # 基准测试数据（自动生成）
│   ├── results/                       # 历史测试结果
│   ├── latest.json                    # 最新结果（JSON）
│   └── latest.md                      # 最新结果（Markdown）
│
└── archive/                           # 归档（历史性文档）
    ├── audit-2025-12-29-docs-style.md
    ├── thinking-process.md
    ├── implementation-progress.md
    └── requirements.md
```

---

## 快速导航

### 新用户入门

1. [快速开始](guide/getting-started.md) — 安装、构建、运行第一个命令
2. [CLI 参考](guide/cli-reference.md) — stat / filter 完整用法
3. [配置管理](guide/configuration.md) — 环境变量、配置文件、命令行参数
4. [部署指南](guide/deployment.md) — Docker 与生产环境

### 开发者指南

1. [架构设计](dev/architecture.md) — 分层架构、技术栈、并发模型
2. [核心设计](dev/design.md) — 数据模型、流水线、日志
3. [构建指南](dev/build.md) — 编译器、CMake、Conan、Sanitizers
4. [测试策略](dev/testing.md) — 单元测试、集成测试、E2E、覆盖率
5. [编码规范](dev/coding-standards.md) — 命名、格式、现代 C++ 实践
6. [Git 工作流](dev/git-guidelines.md) — 提交规范、分支策略
7. [代码质量工具](dev/quality-tools.md) — Sanitizers、Valgrind、静态分析、模糊测试
8. [Benchmark 指南](dev/benchmark-guide.md) — 性能基准测试与报告
9. [DevContainer](dev/devcontainer.md) — 容器化开发环境、SSH 方案
10. [脚本迁移](dev/migration.md) — 从旧脚本系统迁移

### API 参考

1. [API 概览](api/overview.md) — 入口、模块架构、快速示例
2. [IO 模块](api/io.md) — FastqReader / FastqWriter / FastqRecord
3. [处理流水线](api/processing.md) — Pipeline / Predicate / Mutator
4. [统计分析](api/statistics.md) — StatisticCalculator
5. [核心工具](api/core.md) — Configuration / Error / Logging

### 架构决策

1. [工具链版本规范](decisions/toolchain-policy.md) — 编译器、Docker、CMake 版本选型与升级策略
2. [优化决策](decisions/optimization-decisions.md) — 优化需求、辩证分析、实施状态
3. [项目评估](decisions/project-assessment.md) — 架构、性能、依赖、测试全面评估

---

## 文档维护

### 命名约定

- 文档文件：`kebab-case.md`
- 目录名：`lowercase`

### 更新记录

| 日期 | 更新内容 |
|------|----------|
| 2026-03-08 | 全面优化：修复过时版本号、统一构建目录命名、消除命名规范矛盾、去重、翻译 steering 文档 |
| 2026-03-07 | 彻底重构文档目录：消除重复、合并 wiki/、新增 api/ 和 guide/、统一中文 |
| 2025-02-24 | 重构文档目录：合并重复内容、消除碎片目录、引入 decisions/ 和 archive/ |
| 2025-12-30 | 添加项目分析文档，重组目录结构 |
| 2025-12-29 | 文档风格统一 |

---

*如有问题，请提交 Issue 或 PR。*
