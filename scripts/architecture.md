# Scripts 架构设计

## 目录结构

```
scripts/
├── core/                    # 核心脚本（推荐使用）
│   ├── build               # 统一构建脚本
│   ├── test                # 统一测试脚本
│   ├── lint                # 代码质量检查
│   └── install-deps        # 依赖安装
│
├── lib/                     # 公共函数库 / 内部工具
│   ├── common.sh           # 核心工具函数
│   └── gcov-wrapper        # gcov/llvm-cov 包装器
│
├── tools/                   # 工具脚本
│   ├── performance/        # 性能测试与分析
│   │   ├── benchmark
│   │   ├── benchmark-io
│   │   └── valgrind-cachegrind
│   ├── analysis/           # 分析与测试辅助
│   │   ├── coverage-report
│   │   ├── generate-diff
│   │   ├── run-fuzzer
│   │   └── valgrind-memcheck
│   └── release/            # 发布与环境工具
│       ├── deploy
│       ├── install-llvm
│       ├── package-release
│       └── setup-devcontainer
│
└── README.md                # 脚本系统文档
```

## 命名规范

### 核心脚本
- 无扩展名（如 `build`, `test`）
- 使用连字符分隔单词（如 `install-deps`）
- 简短、语义明确

### 工具脚本
- 无扩展名
- 使用连字符分隔单词
- 描述性名称（如 `benchmark-io`, `package-release`）

### 库文件
- 函数库使用 `.sh` 扩展名（如 `common.sh`）
- 可执行包装器无扩展名（如 `gcov-wrapper`）

## 职责划分

### core/ - 核心开发脚本
日常开发必需的脚本，所有开发者都会使用：
- `build` - 项目构建
- `test` - 测试运行
- `lint` - 代码质量
- `install-deps` - 环境配置

### tools/ - 专用工具脚本
特定场景使用的脚本：
- `benchmark` - 性能基准测试 CLI
- `benchmark-io` - I/O 性能测试
- `coverage-report` - 覆盖率报告
- `deploy` - Docker 部署
- `generate-diff` - 差异分析
- `install-llvm` - LLVM 工具链安装
- `package-release` - 发布打包
- `run-fuzzer` - Fuzz 测试
- `setup-devcontainer` - DevContainer 初始化
- `valgrind-cachegrind` - 缓存性能分析
- `valgrind-memcheck` - 内存检查

### lib/ - 公共函数库和内部工具
- `common.sh` - 通用工具函数
- `gcov-wrapper` - gcov/llvm-cov 包装器

## 使用原则

1. **优先使用 core/ 脚本**
   ```bash
   ./scripts/core/build --dev
   ./scripts/core/test --unit
   ```

2. **工具脚本按需使用**
   ```bash
    ./scripts/tools/performance/benchmark-io
    ./scripts/tools/release/package-release 3.1.0
   ```

## 迁移时间线

| 日期 | 操作 |
|------|------|
| 2026-01-08 | 新架构生效 |
| 2026-02-24 | 清理完成：旧脚本删除，所有脚本归入 core/tools/lib |

## 扩展指南

### 添加新的核心脚本
```bash
# 1. 在 core/ 创建脚本
touch scripts/core/my-script
chmod +x scripts/core/my-script

# 2. 使用模板
#!/bin/bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/../lib/common.sh"

# 3. 更新文档
# 在 README.md 中添加说明
```

### 添加新的工具脚本
```bash
# 1. 在 tools 子目录创建脚本
touch scripts/tools/analysis/my-tool
chmod +x scripts/tools/analysis/my-tool

# 2. 添加描述注释
# 3. 可选：添加到 README.md 的工具列表
```
