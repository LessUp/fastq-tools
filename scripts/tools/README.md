# FastQTools 工具脚本

专用工具脚本目录，包含特定场景使用的实用工具。

## 工具列表

### 🏆 benchmark
**性能基准测试 CLI**

统一的性能基准测试命令行接口，支持运行、报告、对比和基线管理。

```bash
./scripts/tools/benchmark run                    # 运行基准测试
./scripts/tools/benchmark run --ci               # CI 模式
./scripts/tools/benchmark report                 # 生成报告
./scripts/tools/benchmark compare a.json b.json  # 对比结果
./scripts/tools/benchmark baseline save v1.0     # 保存基线
./scripts/tools/benchmark data generate          # 生成测试数据
```

---

### 🚀 benchmark-io
**I/O 性能基准测试**

测试 FASTQ 文件读写性能，用于优化和回归测试。

```bash
./scripts/tools/benchmark-io

# 自定义测试
./scripts/tools/benchmark-io --input data/large.fastq --iterations 10
```

**用途**:
- 性能优化验证
- 回归测试
- 不同存储设备对比

---

### 📦 package-release
**发布打包工具**

生成可分发的 FastQTools 发布包。

```bash
# 基本用法
./scripts/tools/package-release 3.1.0

# 输出
# dist/fastqtools-v3.1.0-linux-x86_64.tar.gz
```

**包含内容**:
- 编译后的二进制文件
- 许可证和文档
- Changelog
- 示例文件

**用途**:
- 创建发布版本
- 生成分发包
- CI/CD 自动化

---

### 🚢 deploy
**部署脚本**

部署 FastQTools 到 Docker 或生产环境。

```bash
# Docker 部署
./scripts/tools/deploy docker

# 生产环境部署
./scripts/tools/deploy production --target server1
```

**特点**:
- 多环境支持
- 健康检查
- 回滚机制

**用途**:
- Docker 镜像构建
- 生产环境部署
- CI/CD 集成

---

### 📊 generate-diff
**差异报告生成器**

生成代码变更的详细差异报告，用于审查和文档。

```bash
# 生成未提交变更的差异
./scripts/tools/generate-diff

# 输出
# _output/diffs/diff_workspace_uncommitted_YYYYMMDD-HHMMSS.diff
```

**用途**:
- Code review 准备
- 变更文档
- 归档和审计

---

### 🛠️ setup-devcontainer
**DevContainer 初始化**

配置 VSCode DevContainer 开发环境。

```bash
./scripts/tools/setup-devcontainer
```

**功能**:
- 安装开发工具
- 配置 shell 环境
- 设置 Git hooks

---

### 🔧 install-llvm
**LLVM 工具链安装**

在 Debian/Ubuntu 上安装指定版本的 LLVM 工具链（第三方脚本）。

```bash
sudo ./scripts/tools/install-llvm 21        # 安装 LLVM 21
sudo ./scripts/tools/install-llvm 21 all    # 安装所有包
```

---

### 📊 coverage-report
**覆盖率报告生成**

生成 lcov/HTML 格式的代码覆盖率报告。

```bash
./scripts/tools/coverage-report
```

---

### 🔍 run-fuzzer
**Fuzz 测试运行器**

运行 FASTQ 解析器的 fuzz 测试。

```bash
./scripts/tools/run-fuzzer
```

---

### 🧠 valgrind-memcheck / valgrind-cachegrind
**内存检查与缓存性能分析**

```bash
./scripts/tools/valgrind-memcheck            # 内存泄漏 / 非法访问
./scripts/tools/valgrind-cachegrind          # 缓存命中 / 指令级性能
```

---

## 添加新工具

如果你需要添加新的工具脚本：

1. **创建脚本**
   ```bash
   touch scripts/tools/my-tool
   chmod +x scripts/tools/my-tool
   ```

2. **使用模板**
   ```bash
   #!/bin/bash
   set -euo pipefail
   
   SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
   source "${SCRIPT_DIR}/../lib/common.sh"
   
   # 脚本逻辑
   log_section "我的工具"
   # ...
   ```

3. **添加文档**
   - 在此 README 中添加说明
   - 提供使用示例
   - 说明用途和场景

---

## 工具脚本 vs 核心脚本

### 核心脚本 (`scripts/core/`)
- 日常开发必需
- 所有开发者都会使用
- 频繁调用
- 标准化接口

**示例**: build, test, lint, install-deps

### 工具脚本 (`scripts/tools/`)
- 特定场景使用
- 部分开发者/流程使用
- 偶尔调用
- 专用功能

**示例**: benchmark, package, deploy

---

## 最佳实践

### ✅ 适合作为工具脚本
- 性能测试和基准
- 发布和打包
- 部署和运维
- 数据生成和转换
- 报告生成

### ❌ 不适合作为工具脚本
- 日常编译和测试（应该在 core/）
- 简单的一次性任务（可以手动执行）
- 核心开发流程（应该在 core/）

---

## 维护指南

### 工具脚本要求
1. **独立性**: 不应依赖外部状态
2. **文档**: 提供清晰的使用说明
3. **错误处理**: 优雅处理错误情况
4. **幂等性**: 可重复执行
5. **日志**: 使用 common.sh 的日志函数

### 定期检查
- 检查脚本是否仍然需要
- 更新文档和示例
- 测试功能正常
- 清理过时的工具

---

**维护**: FastQTools 团队  
**更新**: 2026-02-24
