# FastQTools 脚本系统

本文档描述 FastQTools 脚本系统的架构、使用方法和最佳实践。

## 脚本架构

```
scripts/
├── lib/
│   └── common.sh         # 公共函数库（核心）
├── build                 # 统一构建脚本 ⭐
├── test                  # 统一测试脚本 ⭐
├── lint                  # 代码质量检查 ⭐
├── install-deps          # 依赖安装 ⭐
├── package               # 发布打包
├── deploy                # 部署脚本
└── [deprecated]/         # 已废弃脚本
```

**⭐ 标记的脚本是新版统一接口，推荐使用**

---

## 核心脚本

### 1. 构建脚本 (`build`)

统一的项目构建入口，支持多种配置。

#### 基本用法

```bash
# 标准 Release 构建
./scripts/build

# 开发模式（Debug + 详细输出）
./scripts/build --dev

# 指定编译器和构建类型
./scripts/build --compiler gcc --type Debug

# AddressSanitizer 构建
./scripts/build --sanitizer asan

# 覆盖率构建
./scripts/build --coverage
```

#### 高级选项

```bash
# 清理后重新构建
./scripts/build --clean

# 自定义构建目录
./scripts/build --build-dir my-custom-build

# 禁用 LTO
./scripts/build --no-lto

# 静态分析
./scripts/build --static-analysis

# 指定并行任务数
./scripts/build --jobs 8
```

#### 替代的旧脚本

- ❌ `build.sh` → ✅ `build`
- ❌ `build-dev.sh` → ✅ `build --dev`

---

### 2. 测试脚本 (`test`)

统一的测试运行器，支持单元测试、集成测试和覆盖率。

#### 基本用法

```bash
# 运行所有测试
./scripts/test

# 只运行单元测试
./scripts/test --unit

# 只运行集成测试
./scripts/test --integration

# 只运行 E2E 测试
./scripts/test --e2e

# 生成覆盖率报告
./scripts/test --coverage
```

#### 高级选项

```bash
# 过滤测试
./scripts/test --filter "*config*"

# 重复测试（稳定性测试）
./scripts/test --repeat 5

# 详细输出
./scripts/test --verbose

# 自定义构建目录
./scripts/test --build-dir build-gcc-debug

# 并行测试
./scripts/test --jobs 8
```

#### 替代的旧脚本

- ❌ `test.sh` → ✅ `test`

---

### 3. 代码质量脚本 (`lint`)

统一的代码格式化和静态分析工具。

#### 基本用法

```bash
# 检查代码风格
./scripts/lint check

# 自动格式化
./scripts/lint format

# 静态分析
./scripts/lint tidy

# 静态分析并自动修复
./scripts/lint tidy-fix

# 运行所有检查
./scripts/lint all
```

#### 高级选项

```bash
# 指定构建目录（用于 clang-tidy）
./scripts/lint tidy --build-dir build-clang-release

# 详细输出
./scripts/lint --verbose

# 自定义样式文件
./scripts/lint format --style my-style.yaml
```

#### 替代的旧脚本

- ❌ `lint.sh` → ✅ `lint`

---

### 4. 依赖安装脚本 (`install-deps`)

统一的依赖安装和环境配置工具。

#### 基本用法

```bash
# 安装开发依赖
./scripts/install-deps

# 只安装运行时依赖
./scripts/install-deps --runtime

# 安装所有依赖（包括可选工具）
./scripts/install-deps --all
```

#### 高级选项

```bash
# 预览将要安装的包
./scripts/install-deps --dry-run

# 跳过包管理器更新
./scripts/install-deps --skip-update

# 详细输出
./scripts/install-deps --verbose
```

#### 替代的旧脚本

- ❌ `install_deps.sh` → ✅ `install-deps`
- ❌ `install_runtime.sh` → ✅ `install-deps --runtime`

---

## 公共函数库

所有脚本都依赖 `lib/common.sh` 提供的公共函数。

### 日志函数

```bash
source "$(dirname "${BASH_SOURCE[0]}")/lib/common.sh"

log_info "这是信息日志"
log_success "操作成功"
log_warning "这是警告"
log_error "这是错误"
log_debug "调试信息（需要 VERBOSE=true）"
```

### 路径和环境工具

```bash
# 获取项目根目录
PROJECT_ROOT=$(get_project_root)

# 获取标准构建目录名称
BUILD_DIR=$(get_build_dir "clang" "Release")
# 返回: build-clang-release

# 检查命令是否存在
require_command cmake "sudo apt-get install cmake"

# 检查多个命令
require_commands cmake ninja gcc g++
```

### 构建工具

```bash
# 获取 CPU 核心数
CORES=$(get_cpu_cores)

# 标准化构建类型
BUILD_TYPE=$(normalize_build_type "debug")  # 返回: Debug
BUILD_TYPE=$(normalize_build_type "rel")    # 返回: Release

# 标准化编译器
COMPILER=$(normalize_compiler "gnu")        # 返回: gcc
```

### 进度和时间

```bash
# 显示步骤
show_step 1 3 "配置环境"

# 计时
start_timer
# ... 执行耗时操作 ...
end_timer  # 显示: "Completed in 2m 30s"
```

### 用户交互

```bash
# 请求确认
if confirm "继续操作?" "y"; then
    echo "用户确认"
fi
```

---

## 快速开始

### 首次设置

```bash
# 1. 安装依赖
./scripts/install-deps

# 2. 构建项目
./scripts/build --dev

# 3. 运行测试
./scripts/test

# 4. 检查代码质量
./scripts/lint check
```

### 日常开发工作流

```bash
# 开发构建
./scripts/build --dev

# 运行相关测试
./scripts/test --filter "*myfeature*" --verbose

# 格式化代码
./scripts/lint format

# 静态分析
./scripts/lint tidy-fix
```

### 发布前检查

```bash
# 完整的质量检查
./scripts/build --clean
./scripts/lint all
./scripts/test --coverage
./scripts/build --sanitizer asan
./scripts/test
```

---

## 自定义和扩展

### 创建新脚本

如果需要创建新脚本，遵循以下模板：

```bash
#!/bin/bash
# scripts/my-script
# 
# 脚本简短描述

set -euo pipefail

# 加载公共函数库
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "${SCRIPT_DIR}/lib/common.sh"

# 主逻辑
log_section "我的脚本"
start_timer

# 你的代码...

end_timer
```

### 扩展公共函数库

如果有通用的函数，添加到 `lib/common.sh`：

```bash
# 在 common.sh 中添加
my_custom_function() {
    local arg="$1"
    log_info "处理: ${arg}"
    # ...
}

# 导出函数
export -f my_custom_function
```

---

## 故障排查

### 脚本执行失败

1. **权限问题**
   ```bash
   chmod +x scripts/*
   ```

2. **依赖缺失**
   ```bash
   ./scripts/install-deps --all
   ```

3. **查看详细输出**
   ```bash
   ./scripts/build --verbose
   ```

### 常见错误

#### 错误: "Required command not found"

**解决方案**: 安装缺失的依赖
```bash
./scripts/install-deps --dev
```

#### 错误: "Build directory not found"

**解决方案**: 先运行构建
```bash
./scripts/build
./scripts/test
```

#### 错误: "compile_commands.json not found"

**解决方案**: 确保 CMake 导出编译数据库
```bash
./scripts/build --clean
```

---

## 环境变量

脚本支持以下环境变量：

| 变量 | 说明 | 默认值 |
|------|------|--------|
| `VERBOSE` | 详细输出 | `false` |
| `JOBS` | 并行任务数 | 自动检测 |
| `COMPILER` | 默认编译器 | `clang` |
| `BUILD_TYPE` | 默认构建类型 | `Release` |

使用示例：
```bash
VERBOSE=true ./scripts/build
JOBS=16 ./scripts/test
```

---

## CI/CD 集成

### GitHub Actions

```yaml
- name: Build and Test
  run: |
    ./scripts/install-deps --dev
    ./scripts/build --coverage
    ./scripts/test --coverage
    ./scripts/lint all
```

### Docker

```dockerfile
# 在 Dockerfile 中使用
RUN ./scripts/install-deps --runtime
RUN ./scripts/build --type Release
```

---

## 最佳实践

### DO ✅

- 使用新的统一脚本接口
- 在 CI 中始终运行完整检查
- 提交前运行 `./scripts/lint format`
- 编写新功能时先运行 `./scripts/build --dev`
- 使用 `--verbose` 调试问题

### DON'T ❌

- 不要直接调用已废弃的脚本
- 不要跳过代码格式化检查
- 不要在未运行测试时提交代码
- 不要硬编码路径和配置
- 不要忽略 sanitizer 报告

---

## 迁移指南

从旧脚本迁移到新脚本：

| 旧命令 | 新命令 |
|--------|--------|
| `./scripts/build.sh clang Release` | `./scripts/build` |
| `./scripts/build.sh gcc Debug` | `./scripts/build -c gcc -t Debug` |
| `./scripts/build-dev.sh` | `./scripts/build --dev` |
| `./scripts/test.sh -c clang` | `./scripts/test` |
| `./scripts/test.sh -C` | `./scripts/test --coverage` |
| `./scripts/lint.sh format` | `./scripts/lint format` |
| `./scripts/install_deps.sh` | `./scripts/install-deps` |

详细迁移说明请参见 `MIGRATION.md`

---

## 参考

- 测试文档: `tests/README.md`
- 迁移指南: `MIGRATION.md`
- 贡献指南: `CONTRIBUTING.md`
