# FastQTools 核心脚本

这是 FastQTools 的核心开发脚本目录，包含日常开发必需的所有工具。

## 快速开始

```bash
# 开发构建
./scripts/core/build --dev

# 运行测试
./scripts/core/test --unit

# 代码格式化
./scripts/core/lint format

# 安装依赖
./scripts/core/install-deps
```

## 脚本列表

### 🔨 build
**统一构建脚本** - 支持多种编译器、构建类型和分析工具

```bash
# 基本用法
./scripts/core/build                         # Release 构建
./scripts/core/build --dev                   # 开发模式
./scripts/core/build -c gcc -t Debug         # GCC Debug
./scripts/core/build --sanitizer asan        # AddressSanitizer
./scripts/core/build --coverage              # 覆盖率构建
./scripts/core/build --clean                 # 清理后构建

# 查看所有选项
./scripts/core/build --help
```

**特点**:
- 自动检测 CPU 核心数
- 支持多种编译器（gcc, clang）
- 集成 Sanitizer 和覆盖率
- 详细的错误信息和恢复建议
- 进度显示和计时

---

### 🧪 test
**统一测试运行器** - 支持分层测试和覆盖率报告

```bash
# 基本用法
./scripts/core/test                          # 运行所有测试
./scripts/core/test --unit                   # 只运行单元测试
./scripts/core/test --integration            # 只运行集成测试
./scripts/core/test --e2e                    # 只运行 E2E 测试
./scripts/core/test --coverage               # 生成覆盖率报告

# 高级用法
./scripts/core/test --filter "*config*"      # 过滤测试
./scripts/core/test --repeat 5               # 重复测试 5 次
./scripts/core/test --verbose                # 详细输出

# 查看所有选项
./scripts/core/test --help
```

**特点**:
- 三层测试架构（unit/integration/e2e）
- 智能测试过滤
- 覆盖率报告（lcov + HTML）
- 并行测试执行
- 测试重复和超时控制

---

### ✨ lint
**代码质量检查** - 格式化和静态分析

```bash
# 基本用法
./scripts/core/lint check                    # 检查代码风格
./scripts/core/lint format                   # 自动格式化
./scripts/core/lint tidy                     # 静态分析
./scripts/core/lint tidy-fix                 # 自动修复问题
./scripts/core/lint all                      # 运行所有检查

# 高级用法
./scripts/core/lint tidy -b build-clang-release
./scripts/core/lint format --verbose

# 查看所有选项
./scripts/core/lint --help
```

**特点**:
- clang-format 代码格式化
- clang-tidy 静态分析
- 自动修复功能
- 批量处理文件
- CI 友好的输出

---

### 📦 install-deps
**依赖安装和管理** - 自动配置开发环境

```bash
# 基本用法
./scripts/core/install-deps                  # 安装开发依赖
./scripts/core/install-deps --runtime        # 只安装运行时依赖
./scripts/core/install-deps --all            # 安装所有依赖（包括可选工具）

# 高级用法
./scripts/core/install-deps --dry-run        # 预览将要安装的包
./scripts/core/install-deps --skip-update    # 跳过包管理器更新
./scripts/core/install-deps --verbose        # 详细输出

# 查看所有选项
./scripts/core/install-deps --help
```

**特点**:
- 智能系统检测
- 分层依赖管理（runtime/dev/all）
- 安装验证
- Dry-run 模式
- Python 工具集成

---

## 日常工作流

### 新功能开发
```bash
# 1. 安装依赖（首次）
./scripts/core/install-deps

# 2. 开发构建
./scripts/core/build --dev

# 3. 运行相关测试
./scripts/core/test --unit --filter "*myfeature*"

# 4. 代码格式化
./scripts/core/lint format

# 5. 静态检查
./scripts/core/lint tidy-fix
```

### 提交前检查
```bash
# 完整的质量保证流程
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

---

## 环境变量

所有脚本支持以下环境变量：

| 变量 | 说明 | 默认值 |
|------|------|--------|
| `VERBOSE` | 详细输出 | `false` |
| `JOBS` | 并行任务数 | 自动检测 |
| `COMPILER` | 默认编译器 | `clang` |
| `BUILD_TYPE` | 默认构建类型 | `Release` |

使用示例：
```bash
VERBOSE=true ./scripts/core/build
JOBS=16 ./scripts/core/test
```

---

## 公共函数库

所有核心脚本都依赖 `scripts/lib/common.sh` 提供的公共函数。

主要功能：
- 统一的日志系统
- 错误处理和恢复
- 路径和环境管理
- 构建工具函数
- 系统检测

详见：`scripts/lib/common.sh`

---

## 故障排查

### 脚本无法执行
```bash
# 赋予执行权限
chmod +x scripts/core/*
```

### 依赖缺失
```bash
# 重新安装依赖
./scripts/core/install-deps --all
```

### 构建失败
```bash
# 查看详细输出
./scripts/core/build --verbose

# 清理后重试
./scripts/core/build --clean
```

### 测试失败
```bash
# 查看详细日志
./scripts/core/test --verbose

# 只运行失败的测试
./scripts/core/test --filter "*FailingTest*"
```

---

## 更多文档

- **完整脚本文档**: `scripts/README.md`
- **架构设计**: `scripts/ARCHITECTURE.md`
- **测试文档**: `tests/README.md`
- **迁移指南**: `MIGRATION.md`

---

## 最佳实践

### ✅ DO
- 使用 `--help` 查看所有选项
- 提交前运行 `lint format`
- 开发时使用 `--dev` 模式
- 失败时查看 `--verbose` 输出

### ❌ DON'T
- 不要使用 `scripts/deprecated/` 中的脚本
- 不要跳过代码格式化
- 不要忽略 sanitizer 警告
- 不要硬编码路径

---

**维护**: FastQTools 团队  
**更新**: 2026-01-08  
**版本**: 2.0
