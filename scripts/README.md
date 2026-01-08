# FastQTools 脚本系统

FastQTools 的脚本系统提供统一、清晰的构建、测试和部署工具。

## 📁 目录结构

```
scripts/
├── core/                    # ⭐ 核心脚本（日常开发）
│   ├── build               # 统一构建脚本
│   ├── test                # 统一测试运行器
│   ├── lint                # 代码质量检查
│   ├── install-deps        # 依赖安装
│   └── README.md           # 核心脚本文档
│
├── tools/                   # 🛠️ 工具脚本（特定场景）
│   ├── benchmark-io        # I/O 性能基准测试
│   ├── package-release     # 发布打包
│   ├── deploy              # 部署脚本
│   ├── generate-diff       # 差异报告生成
│   ├── setup-devcontainer  # DevContainer 初始化
│   └── README.md           # 工具脚本文档
│
├── lib/                     # 📚 公共函数库
│   └── common.sh           # 核心工具函数
│
├── deprecated/              # ⚠️ 已废弃脚本（仅兼容）
│   ├── build.sh            # → 使用 core/build
│   ├── test.sh             # → 使用 core/test
│   ├── lint.sh             # → 使用 core/lint
│   └── README.md           # 废弃说明
│
├── README.md               # 本文件
└── ARCHITECTURE.md         # 架构设计文档
```

## 🚀 快速开始

### 首次设置

```bash
# 1. 安装开发依赖
./scripts/core/install-deps

# 2. 开发构建
./scripts/core/build --dev

# 3. 运行测试
./scripts/core/test --unit

# 4. 代码格式化
./scripts/core/lint format
```

### 日常开发

```bash
# 快速构建和测试
./scripts/core/build --dev
./scripts/core/test --filter "*myfeature*"

# 提交前检查
./scripts/core/lint all
./scripts/core/test --coverage
```

## 📖 核心脚本

### build - 统一构建脚本

```bash
./scripts/core/build                         # Release 构建
./scripts/core/build --dev                   # 开发模式
./scripts/core/build -c gcc -t Debug         # GCC Debug
./scripts/core/build --sanitizer asan        # AddressSanitizer
./scripts/core/build --coverage              # 覆盖率构建
./scripts/core/build --help                  # 查看所有选项
```

### test - 统一测试运行器

```bash
./scripts/core/test                          # 所有测试
./scripts/core/test --unit                   # 单元测试
./scripts/core/test --integration            # 集成测试
./scripts/core/test --e2e                    # 端到端测试
./scripts/core/test --coverage               # 覆盖率报告
./scripts/core/test --filter "*config*"      # 过滤测试
./scripts/core/test --help                   # 查看所有选项
```

### lint - 代码质量检查

```bash
./scripts/core/lint check                    # 检查代码风格
./scripts/core/lint format                   # 自动格式化
./scripts/core/lint tidy                     # 静态分析
./scripts/core/lint tidy-fix                 # 自动修复
./scripts/core/lint all                      # 运行所有检查
./scripts/core/lint --help                   # 查看所有选项
```

### install-deps - 依赖管理

```bash
./scripts/core/install-deps                  # 开发依赖
./scripts/core/install-deps --runtime        # 运行时依赖
./scripts/core/install-deps --all            # 所有依赖
./scripts/core/install-deps --dry-run        # 预览安装
./scripts/core/install-deps --help           # 查看所有选项
```

详细文档：`scripts/core/README.md`

## 🛠️ 工具脚本

### benchmark-io - 性能基准测试
```bash
./scripts/tools/benchmark-io
```

### package-release - 发布打包
```bash
./scripts/tools/package-release 3.1.0
```

### deploy - 部署
```bash
./scripts/tools/deploy docker
```

### generate-diff - 差异报告
```bash
./scripts/tools/generate-diff
```

### setup-devcontainer - DevContainer 初始化
```bash
./scripts/tools/setup-devcontainer
```

详细文档：`scripts/tools/README.md`

## ⚠️ 废弃脚本

`scripts/deprecated/` 目录包含旧版脚本，仅供兼容性保留。

**请勿使用！** 这些脚本将在 2026-07-01 完全移除。

| 废弃脚本 | 替代方案 |
|----------|----------|
| `build.sh` | `core/build` |
| `build-dev.sh` | `core/build --dev` |
| `test.sh` | `core/test` |
| `lint.sh` | `core/lint` |
| `install_deps.sh` | `core/install-deps` |
| `install_runtime.sh` | `core/install-deps --runtime` |

迁移指南：`../MIGRATION.md`

## 📚 公共函数库

`scripts/lib/common.sh` 提供所有脚本共用的函数：

- **日志**: `log_info`, `log_success`, `log_error`, `log_warning`
- **错误处理**: `error_handler`, `setup_error_trap`
- **路径工具**: `get_project_root`, `get_build_dir`
- **构建工具**: `normalize_build_type`, `get_cpu_cores`
- **系统检测**: `detect_os`, `detect_distro`
- **进度显示**: `show_step`, `start_timer`, `end_timer`

## 🎯 工作流示例

### 开发新功能

```bash
# 1. 开发构建
./scripts/core/build --dev

# 2. 运行相关测试
./scripts/core/test --unit --filter "*myfeature*" -v

# 3. 代码格式化
./scripts/core/lint format

# 4. 静态分析
./scripts/core/lint tidy-fix
```

### 提交前完整检查

```bash
# 完整质量保证
./scripts/core/lint all
./scripts/core/build --clean
./scripts/core/test --coverage
```

### CI/CD 集成

```bash
# 标准 CI 流程
./scripts/core/install-deps --dev
./scripts/core/build --coverage
./scripts/core/test --coverage
./scripts/core/lint all
```

### 发布流程

```bash
# 1. 完整测试
./scripts/core/build --clean
./scripts/core/test --all

# 2. 多配置验证
./scripts/core/build -c gcc -t Release
./scripts/core/build --sanitizer asan

# 3. 生成发布包
./scripts/tools/package-release 3.1.0
```

## 🎨 设计原则

### 1. 清晰的职责分离
- **core/** - 日常必需的核心功能
- **tools/** - 特定场景的专用工具
- **lib/** - 可复用的公共函数
- **deprecated/** - 废弃但保留兼容的脚本

### 2. 统一的接口规范
- 无扩展名的可执行文件
- 长选项格式（`--option`）
- 必需的 `--help` 支持
- 一致的日志和错误处理

### 3. 模块化和可复用
- 公共函数库避免重复
- 脚本间可相互调用
- 环境变量统一管理

### 4. 完善的文档
- 每个目录都有 README
- 详细的使用示例
- 故障排查指南

## 📄 相关文档

- **架构设计**: `scripts/ARCHITECTURE.md`
- **核心脚本**: `scripts/core/README.md`
- **工具脚本**: `scripts/tools/README.md`
- **测试系统**: `../tests/README.md`
- **迁移指南**: `../MIGRATION.md`

## 🔧 开发指南

### 添加新的核心脚本

```bash
# 1. 创建脚本
touch scripts/core/my-script
chmod +x scripts/core/my-script

# 2. 使用模板
cat > scripts/core/my-script << 'EOF'
#!/bin/bash
set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/../lib/common.sh"

# 脚本逻辑
log_section "My Script"
# ...
EOF

# 3. 更新文档
# 在 core/README.md 中添加说明
```

### 添加公共函数

```bash
# 编辑 scripts/lib/common.sh
# 添加函数并导出
my_function() {
    # ...
}
export -f my_function
```

## ⚙️ 环境变量

| 变量 | 说明 | 默认值 |
|------|------|--------|
| `VERBOSE` | 详细输出 | `false` |
| `JOBS` | 并行任务数 | 自动检测 |
| `COMPILER` | 默认编译器 | `clang` |
| `BUILD_TYPE` | 默认构建类型 | `Release` |

## 🐛 故障排查

### 权限错误
```bash
chmod +x scripts/core/*
chmod +x scripts/tools/*
```

### 依赖缺失
```bash
./scripts/core/install-deps --all
```

### 查看详细日志
```bash
./scripts/core/build --verbose
./scripts/core/test --verbose
```

## 📊 版本历史

- **v2.0** (2026-01-08): 重大架构重构
  - 引入 core/tools/deprecated 分层
  - 创建公共函数库
  - 统一脚本接口

- **v1.0** (早期): 原始脚本系统
  - 分散的 .sh 脚本
  - 重复代码

## 🤝 贡献

添加或修改脚本时：
1. 遵循命名规范
2. 使用 `lib/common.sh` 函数
3. 添加完整的 `--help` 信息
4. 更新相关文档
5. 添加使用示例

---

**维护**: FastQTools 团队  
**更新**: 2026-01-08  
**版本**: 2.0
