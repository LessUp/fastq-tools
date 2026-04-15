# FastQTools 文档中心

> 本目录包含 FastQTools 项目的所有文档资源，按受众和用途组织。

---

## 目录结构

```
docs/
├── index.md                           # 文档首页
├── README.md                          # 本文件
│
├── guide/                             # 用户指南
│   ├── getting-started.md             # 快速开始
│   ├── cli-reference.md               # CLI 参考
│   ├── configuration.md               # 配置管理
│   └── deployment.md                  # 部署指南
│
├── dev/                               # 开发者文档
│   ├── architecture.md                # 架构设计
│   ├── design.md                      # 核心设计
│   ├── build.md                       # 构建指南
│   ├── testing.md                     # 测试策略
│   ├── coding-standards.md            # 编码规范
│   ├── git-guidelines.md              # Git 规范
│   ├── quality-tools.md               # 质量工具
│   ├── benchmark-guide.md             # 性能基准
│   ├── devcontainer.md                # DevContainer
│   └── migration.md                   # 迁移指南
│
├── api/                               # API 参考
│   ├── overview.md                    # API 概览
│   ├── io.md                          # IO 模块
│   ├── processing.md                  # 处理流水线
│   ├── statistics.md                  # 统计分析
│   └── core.md                        # 核心工具
│
├── decisions/                         # 架构决策记录
│   └── toolchain-policy.md            # 工具链规范
│
├── specs/                             # 功能规格
│   └── ...
│
├── benchmark-reports/                 # 基准测试报告
│   └── ...
│
└── archive/                           # 归档文档
    └── ...
```

---

## 快速导航

### 新用户入门

1. [快速开始](guide/getting-started.md) — 安装、构建、运行
2. [CLI 参考](guide/cli-reference.md) — stat / filter 用法
3. [部署指南](guide/deployment.md) — Docker 与生产环境

### 开发者指南

1. [架构设计](dev/architecture.md) — 分层架构、技术栈
2. [构建指南](dev/build.md) — 编译器、CMake、Conan
3. [测试策略](dev/testing.md) — 单元测试、集成测试
4. [编码规范](dev/coding-standards.md) — 命名、格式
5. [Git 规范](dev/git-guidelines.md) — 提交规范
6. [质量工具](dev/quality-tools.md) — Sanitizers、Valgrind

### API 参考

1. [API 概览](api/overview.md) — 模块架构
2. [IO 模块](api/io.md) — FastqReader / FastqWriter
3. [处理流水线](api/processing.md) — Pipeline / Predicate / Mutator
4. [统计分析](api/statistics.md) — StatisticCalculator
5. [核心工具](api/core.md) — Configuration / Error / Logging

---

## 文档维护

### 命名约定

- 文档文件：`kebab-case.md`
- 目录名：`lowercase`

### 语言

- 主要使用中文
- 提供英文版本时命名为 `*.en.md`
