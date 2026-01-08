# 快速开始指南

5 分钟快速上手 FastQTools 脚本系统。

## 🎯 核心概念

FastQTools 脚本分为三类：

- **core/** - 日常开发必需（你会经常用）
- **tools/** - 特定场景工具（偶尔用）
- **deprecated/** - 旧版脚本（别用）

## 🚀 新手快速上手

### 1. 首次设置（5分钟）

```bash
# 克隆项目后运行
cd fastq-tools

# 安装依赖
./scripts/core/install-deps

# 首次构建
./scripts/core/build --dev

# 运行测试验证
./scripts/core/test --unit
```

### 2. 日常开发循环

```bash
# 修改代码后...

# 快速重新编译
./scripts/core/build --dev

# 运行相关测试
./scripts/core/test --filter "*mychange*"

# 提交前格式化
./scripts/core/lint format
```

### 3. 提交前检查

```bash
# 完整质量保证（2分钟）
./scripts/core/lint all
./scripts/core/test --coverage
```

## 📖 常用命令速查

### 构建

```bash
# 开发构建（最常用）
./scripts/core/build --dev

# Release 构建
./scripts/core/build

# 清理后构建
./scripts/core/build --clean

# 使用 GCC
./scripts/core/build -c gcc -t Debug
```

### 测试

```bash
# 快速单元测试
./scripts/core/test --unit

# 只测试某个模块
./scripts/core/test --filter "*config*"

# 生成覆盖率
./scripts/core/test --coverage
```

### 代码质量

```bash
# 自动格式化（提交前必做）
./scripts/core/lint format

# 检查代码风格
./scripts/core/lint check

# 静态分析
./scripts/core/lint tidy
```

## 💡 常见问题

### Q: 脚本无法执行？
```bash
chmod +x scripts/core/*
```

### Q: 找不到依赖？
```bash
./scripts/core/install-deps --all
```

### Q: 构建失败？
```bash
# 查看详细错误
./scripts/core/build --verbose

# 清理后重试
./scripts/core/build --clean
```

### Q: 测试失败？
```bash
# 查看详细日志
./scripts/core/test --verbose

# 只运行失败的测试
./scripts/core/test --filter "*FailingTest*"
```

## 🎓 下一步学习

- **完整文档**: `scripts/README.md`
- **核心脚本详解**: `scripts/core/README.md`
- **测试系统**: `tests/README.md`
- **架构设计**: `scripts/ARCHITECTURE.md`

## ⚠️ 重要提示

### ✅ DO
- 使用 `scripts/core/` 中的脚本
- 提交前运行 `lint format`
- 失败时查看 `--verbose` 输出
- 使用 `--help` 查看选项

### ❌ DON'T
- **不要**使用 `scripts/deprecated/` 中的脚本
- **不要**跳过代码格式化
- **不要**忽略测试失败
- **不要**使用旧的 `.sh` 脚本

## 📞 获取帮助

```bash
# 每个脚本都有帮助
./scripts/core/build --help
./scripts/core/test --help
./scripts/core/lint --help
```

或查看文档：`scripts/README.md`

---

**5 分钟上手完成！** 🎉

现在你可以开始开发了。记住：**core/** 目录是你的好朋友。
