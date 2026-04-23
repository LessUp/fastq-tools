# 构建指南

## 系统要求

- **编译器**: GCC 15 / 现代 Clang（开发与生产统一；本地 Clang 构建由脚本按已安装版本适配 Conan）
- **CMake**: 3.28+
- **版本规范**: 详见 `openspec/baseline/architecture/0002-toolchain-policy.md`
- **并行库**: Intel oneTBB（必需，流水线实现依赖）
- **压缩库**: zlib / bzip2 / xz
- **内存**: 建议 4GB+ RAM
- **存储**: 1GB+ 可用空间

## 快速构建

```bash
# 一键构建（Clang + Release）
./scripts/core/build

# 指定编译器和配置
./scripts/core/build --compiler gcc --type Debug

# 启用 sanitizers
./scripts/core/build --sanitizer asan --dev
```

## 依赖管理

```bash
# 安装依赖（首次配置）
./scripts/core/install-deps

# 重新构建（推荐：使用脚本统一管理构建目录）
./scripts/core/build
```

> 说明：早期实验过 `mimalloc` 作为分配器，但目前构建和运行时已不再依赖它，所有配置文件均已移除相关要求。

## 代码质量

```bash
# 格式化代码
./scripts/core/lint format

# 检查格式
./scripts/core/lint check

# 静态分析
./scripts/core/lint tidy
```

## 测试

```bash
# 运行测试
./scripts/core/test

# 覆盖率测试
./scripts/core/build --coverage --dev
./scripts/core/test --coverage
```
## 工具与调试

### Sanitizers (内存与运行时检查)

FastQTools 支持多种 LLVM/GCC Sanitizers，用于检测内存错误和未定义行为：

- **AddressSanitizer (ASan)**: 检测内存泄漏、缓冲区溢出、Use-after-free 等。
- **UndefinedBehaviorSanitizer (USan)**: 检测整数溢出、无效类型转换、空指针解引用等。
- **ThreadSanitizer (TSan)**: 检测多线程竞态条件 (Data Races)。

使用方法：

```bash
# 启用 ASan（推荐在 Debug 模式下使用）
./scripts/core/build --sanitizer asan --dev

# 启用 TSan（注意：TSan 不能与 ASan 同时启用）
./scripts/core/build --sanitizer tsan --dev

# 启用 UBSan
./scripts/core/build --sanitizer ubsan --dev
```

运行编译后的程序（如单元测试）时，Sanitizers 会在检测到问题时自动输出错误报告并终止程序。

### 静态分析

项目集成了 `clang-tidy` 进行静态代码检查：

```bash
# 运行 clang-tidy 静态分析
./scripts/core/lint tidy

# 运行所有检查（format + tidy + cppcheck）
./scripts/core/lint all
```
