# FastQTools 技术栈

## 语言与标准

- C++23（必需）
- 编译器：GCC 15 / Clang 21（开发与生产统一）

## 构建系统

- CMake 3.28+
- Conan 2.x 依赖管理
- Ninja（推荐生成器）

## 依赖

| 库 | 用途 |
|---------|---------|
| Intel oneTBB | 并行流水线处理 |
| spdlog | 日志 |
| fmt | 字符串格式化 |
| cxxopts | 命令行参数解析 |
| zlib-ng/bzip2/xz | 压缩支持 |
| libdeflate | 高性能 gzip 解压 |
| GoogleTest | 单元测试 |

## 常用命令

### 构建

```bash
# Default build (Clang + Release)
./scripts/core/build

# Development build (Debug)
./scripts/core/build --dev

# Specific compiler/type
./scripts/core/build -c gcc -t Debug

# With sanitizers
./scripts/core/build --sanitizer asan
```

### 测试

```bash
# All tests
./scripts/core/test

# Unit tests only
./scripts/core/test --unit

# With coverage
./scripts/core/test --coverage

# Filter specific tests
./scripts/core/test --filter "*config*"
```

### 代码质量

```bash
# Format code
./scripts/core/lint format

# Check formatting
./scripts/core/lint check

# Static analysis (clang-tidy)
./scripts/core/lint tidy

# Cppcheck analysis
./scripts/core/lint cppcheck

# Include-What-You-Use
./scripts/core/lint iwyu

# All checks (format + tidy + cppcheck)
./scripts/core/lint all
```

### Sanitizers（运行时检查）

```bash
# AddressSanitizer (memory errors)
./scripts/core/build --sanitizer asan --dev

# ThreadSanitizer (data races)
./scripts/core/build --sanitizer tsan --dev

# UndefinedBehaviorSanitizer
./scripts/core/build --sanitizer ubsan --dev

# MemorySanitizer (Clang only)
./scripts/core/build --sanitizer msan --dev -c clang
```

### Valgrind（深度分析）

```bash
# Run tests with Valgrind
./scripts/core/test --valgrind

# Memcheck analysis
./scripts/tools/valgrind-memcheck

# Cache performance analysis
./scripts/tools/valgrind-cachegrind
```

### 覆盖率

```bash
# Build with coverage
./scripts/core/build --coverage --dev

# Run tests and generate report
./scripts/core/test --coverage
./scripts/tools/coverage-report

# Generate XML for CI
./scripts/tools/coverage-report --xml
```

### 模糊测试

```bash
# Build and run fuzzers
./scripts/tools/run-fuzzer --build

# Run specific fuzzer
./scripts/tools/run-fuzzer parser -t 300
```

### 依赖安装

```bash
# Install dev dependencies
./scripts/core/install-deps

# Install all dependencies
./scripts/core/install-deps --all
```

## 构建目录

- `build/clang-release/` - Clang Release builds
- `build/clang-debug/` - Clang Debug builds
- `build/gcc-release/` - GCC Release builds
- `build/gcc-debug/` - GCC Debug builds
