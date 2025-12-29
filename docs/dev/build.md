# 构建指南

## 系统要求

- **编译器**: GCC 11+ 或 Clang 19（推荐与 CI/Dev Container 保持一致）
- **CMake**: 3.20+
- **并行库**: Intel oneTBB（必需，流水线实现依赖）
- **压缩库**: zlib / bzip2 / xz
- **内存**: 建议 4GB+ RAM
- **存储**: 1GB+ 可用空间

## 快速构建

```bash
# 一键构建（Clang + Release）
./scripts/build.sh

# 指定编译器和配置
./scripts/build.sh gcc Debug

# 启用 sanitizers
./scripts/build.sh clang Debug --asan
```

## 依赖管理

```bash
# 安装依赖（首次配置）
./scripts/install_deps.sh

# 重新构建（推荐：使用脚本统一管理构建目录）
./scripts/build.sh clang Release
```

> 说明：早期实验过 `mimalloc` 作为分配器，但目前构建和运行时已不再依赖它，所有配置文件均已移除相关要求。

## 代码质量

```bash
# 格式化代码
./scripts/lint.sh format

# 检查格式
./scripts/lint.sh format-check

# 静态分析
./scripts/lint.sh
```

## 测试

```bash
# 运行测试
./scripts/test.sh

# 覆盖率测试
./scripts/build.sh gcc Debug --coverage
./scripts/test.sh -c clang -t Coverage -C
```
## 工具与调试

### Sanitizers (内存与运行时检查)

FastQTools 支持多种 LLVM/GCC Sanitizers，用于检测内存错误和未定义行为：

- **AddressSanitizer (ASan)**: 检测内存泄漏、缓冲区溢出、Use-after-free 等。
- **UndefinedBehaviorSanitizer (USan)**: 检测整数溢出、无效类型转换、空指针解引用等。
- **ThreadSanitizer (TSan)**: 检测多线程竞态条件 (Data Races)。

使用方法：

```bash
# 启用 ASan 和 USan (推荐在 Debug 模式下组合使用)
./scripts/build.sh clang Debug --asan --usan

# 启用 TSan (注意：TSan 不能与 ASan 同时启用)
./scripts/build.sh clang Debug --tsan
```

运行编译后的程序（如单元测试）时，Sanitizers 会在检测到问题时自动输出错误报告并终止程序。

### 静态分析

项目集成了 `clang-tidy` 进行静态代码检查：

```bash
# 在构建脚本中启用静态分析
./scripts/build.sh clang Debug --static

# 或直接运行 lint 脚本
./scripts/lint.sh lint
```
