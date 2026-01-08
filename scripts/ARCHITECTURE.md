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
├── lib/                     # 公共函数库
│   └── common.sh           # 核心工具函数
│
├── tools/                   # 工具脚本
│   ├── benchmark-io        # I/O 性能基准测试
│   ├── package-release     # 发布打包
│   ├── deploy              # 部署脚本
│   ├── generate-diff       # 生成差异报告
│   └── setup-devcontainer  # DevContainer 初始化
│
├── deprecated/              # 已废弃脚本（仅兼容）
│   ├── build.sh            ⚠️ 使用 core/build
│   ├── build-dev.sh        ⚠️ 使用 core/build --dev
│   ├── test.sh             ⚠️ 使用 core/test
│   ├── lint.sh             ⚠️ 使用 core/lint
│   ├── install_deps.sh     ⚠️ 使用 core/install-deps
│   └── install_runtime.sh  ⚠️ 使用 core/install-deps --runtime
│
└── README.md                # 脚本系统文档
```

## 命名规范

### 核心脚本
- 无扩展名（如 `build`, `test`）
- 使用连字符分隔单词（如 `install-deps`）
- 简短、语义明确

### 工具脚本
- 无扩展名或 `.sh`
- 使用连字符分隔单词
- 描述性名称（如 `benchmark-io`, `package-release`）

### 库文件
- `.sh` 扩展名
- 小写加下划线（如 `common.sh`）

### 废弃脚本
- 保持原有命名
- 添加废弃警告
- 定期清理

## 职责划分

### core/ - 核心开发脚本
日常开发必需的脚本，所有开发者都会使用：
- `build` - 项目构建
- `test` - 测试运行
- `lint` - 代码质量
- `install-deps` - 环境配置

### tools/ - 专用工具脚本
特定场景使用的脚本：
- `benchmark-io` - 性能测试
- `package-release` - 发布流程
- `deploy` - 部署脚本
- `generate-diff` - 差异分析

### lib/ - 公共函数库
被其他脚本引用的函数库：
- `common.sh` - 通用工具函数

### deprecated/ - 废弃脚本
旧版脚本，仅用于兼容性：
- 显示警告信息
- 重定向到新脚本
- 定期清理

## 使用原则

1. **优先使用 core/ 脚本**
   ```bash
   ./scripts/core/build --dev
   ./scripts/core/test --unit
   ```

2. **避免使用 deprecated/ 脚本**
   ```bash
   # ❌ 不推荐
   ./scripts/deprecated/build.sh
   
   # ✅ 推荐
   ./scripts/core/build
   ```

3. **工具脚本按需使用**
   ```bash
   ./scripts/tools/benchmark-io
   ./scripts/tools/package-release 3.1.0
   ```

## 迁移时间线

| 日期 | 操作 |
|------|------|
| 2026-01-08 | 新架构生效 |
| 2026-02-01 | deprecated/ 脚本显示警告 |
| 2026-04-01 | 移除 deprecated/ 脚本的功能实现 |
| 2026-07-01 | 完全删除 deprecated/ 目录 |

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
# 1. 在 tools/ 创建脚本
touch scripts/tools/my-tool
chmod +x scripts/tools/my-tool

# 2. 添加描述注释
# 3. 可选：添加到 README.md 的工具列表
```
