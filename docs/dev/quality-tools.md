# Code Quality Tools Guide

本文档介绍 FastQTools 项目中使用的代码质量工具，包括安装、使用和输出解读。

## 目录

- [概述](#概述)
- [Sanitizers](#sanitizers)
- [Valgrind](#valgrind)
- [静态分析](#静态分析)
- [代码覆盖率](#代码覆盖率)
- [模糊测试](#模糊测试)
- [常见问题](#常见问题)

## 概述

| 工具类别 | 工具 | 用途 | 运行时机 |
|---------|------|------|---------|
| 内存检查 | ASan | 内存错误检测 | 每次 PR |
| 内存检查 | MSan | 未初始化内存 | 手动 |
| 线程检查 | TSan | 数据竞争检测 | 手动 |
| 未定义行为 | UBSan | UB 检测 | 手动 |
| 深度分析 | Valgrind | 内存泄漏、缓存分析 | 每周 |
| 静态分析 | clang-tidy | 代码质量 | 每次 PR |
| 静态分析 | Cppcheck | 额外检查 | 每次 PR |
| 覆盖率 | lcov | 测试覆盖率 | 每次 PR |
| 安全测试 | libFuzzer | 模糊测试 | 手动 |

## Sanitizers

### 安装

Sanitizers 是编译器内置功能，无需额外安装。确保使用 GCC 11+ 或 Clang 12+。

### 使用

```bash
# AddressSanitizer - 检测内存错误
./scripts/core/build --sanitizer asan --dev
./scripts/core/test -b build-clang-debug

# ThreadSanitizer - 检测数据竞争
./scripts/core/build --sanitizer tsan --dev
./scripts/core/test -b build-clang-debug

# UndefinedBehaviorSanitizer - 检测未定义行为
./scripts/core/build --sanitizer ubsan --dev
./scripts/core/test -b build-clang-debug

# MemorySanitizer - 检测未初始化内存 (仅 Clang)
./scripts/core/build --sanitizer msan --dev -c clang
./scripts/core/test -b build-clang-debug
```

### 配置选项

环境变量配置文件位于 `config/sanitizers/`:

```bash
# 使用配置文件
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=0:print_stats=1"
export TSAN_OPTIONS="halt_on_error=0:second_deadlock_stack=1"
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=0"
```

### 输出解读

**ASan 错误示例:**
```
==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x...
READ of size 1 at 0x... thread T0
    #0 0x... in function_name file.cpp:123
    #1 0x... in main main.cpp:45
```

- `heap-buffer-overflow`: 堆缓冲区溢出
- `stack-buffer-overflow`: 栈缓冲区溢出
- `use-after-free`: 释放后使用
- `double-free`: 重复释放

**TSan 错误示例:**
```
WARNING: ThreadSanitizer: data race (pid=12345)
  Write of size 8 at 0x... by thread T1:
    #0 function_name file.cpp:100
  Previous read of size 8 at 0x... by thread T2:
    #0 other_function file.cpp:200
```

## Valgrind

### 安装

```bash
# Ubuntu/Debian
sudo apt-get install valgrind

# macOS
brew install valgrind
```

### 使用

```bash
# 使用 Valgrind 运行测试
./scripts/core/test -b build-clang-debug --valgrind

# 手动运行 Memcheck
./scripts/tools/valgrind-memcheck

# 缓存性能分析
./scripts/tools/valgrind-cachegrind
```

### 输出解读

**内存泄漏报告:**
```
==12345== LEAK SUMMARY:
==12345==    definitely lost: 100 bytes in 1 blocks
==12345==    indirectly lost: 0 bytes in 0 blocks
==12345==    possibly lost: 50 bytes in 2 blocks
==12345==    still reachable: 1,000 bytes in 10 blocks
```

- `definitely lost`: 确定泄漏，必须修复
- `indirectly lost`: 间接泄漏，通常随 definitely lost 一起修复
- `possibly lost`: 可能泄漏，需要检查
- `still reachable`: 程序结束时仍可访问，通常是全局对象

### 抑制规则

已知的第三方库误报在 `config/valgrind/suppressions.supp` 中配置。

## 静态分析

### clang-tidy

```bash
# 运行 clang-tidy
./scripts/core/lint tidy

# 自动修复
./scripts/core/lint tidy-fix
```

配置文件: `.clang-tidy`

### Cppcheck

```bash
# 安装
sudo apt-get install cppcheck

# 运行
./scripts/core/lint cppcheck
```

配置文件: `config/cppcheck/cppcheck.cfg`

### Include-What-You-Use

```bash
# 安装
sudo apt-get install iwyu

# 运行
./scripts/core/lint iwyu
```

映射文件: `config/iwyu/mappings.imp`

### 运行所有检查

```bash
./scripts/core/lint all
```

## 代码覆盖率

### 安装

```bash
# Ubuntu/Debian
sudo apt-get install lcov

# 可选: Cobertura XML 生成
pip install gcovr
```

### 使用

```bash
# 构建并运行测试
./scripts/core/build --coverage --dev
./scripts/core/test -b build-clang-debug

# 生成报告
./scripts/tools/coverage-report

# 生成 XML 报告 (CI 用)
./scripts/tools/coverage-report --xml

# 检查阈值
./scripts/tools/coverage-report --check-threshold
```

### 输出

- HTML 报告: `coverage_report/html/index.html`
- XML 报告: `coverage_report/coverage.xml`

### 阈值配置

编辑 `config/coverage/thresholds.json`:

```json
{
  "line_coverage": { "minimum": 70, "target": 80 },
  "function_coverage": { "minimum": 80, "target": 90 }
}
```

## 模糊测试

### 安装

模糊测试需要 Clang 编译器和 libFuzzer (Clang 内置)。

### 使用

```bash
# 构建并运行
./scripts/tools/run-fuzzer --build

# 运行特定 fuzzer
./scripts/tools/run-fuzzer parser -t 300

# 并行运行
./scripts/tools/run-fuzzer -j 4 -t 3600
```

### 语料库

种子文件位于 `tools/fuzz/corpus/`。添加新的种子文件可以提高模糊测试效率。

### 崩溃分析

当 fuzzer 发现崩溃时:

1. 崩溃输入保存在 `build-fuzz/fuzz_artifacts/`
2. 使用崩溃输入重现问题: `./fuzzers/fastq_parser_fuzzer crash-xxx`
3. 修复后将崩溃输入添加到语料库作为回归测试

## 常见问题

### Q: ASan 报告 "LeakSanitizer: detected memory leaks" 但代码看起来正确?

A: 可能是第三方库的已知问题。检查是否在抑制文件中，或添加新的抑制规则。

### Q: TSan 报告误报?

A: TSan 可能对某些同步原语不完全理解。使用 `__tsan_acquire` 和 `__tsan_release` 注解，或添加抑制规则。

### Q: Valgrind 运行太慢?

A: Valgrind 通常比正常运行慢 10-50 倍。对于日常开发，使用 ASan 更快。Valgrind 适合定期深度检查。

### Q: 覆盖率报告显示 0%?

A: 确保:
1. 使用 `--coverage` 构建
2. 运行了测试
3. 测试实际执行了被测代码

### Q: Fuzzer 没有发现任何问题?

A: 这是好事！但也可能是:
1. 运行时间太短，增加 `-t` 参数
2. 语料库不够好，添加更多种子文件
3. 输入长度限制太小，增加 `-l` 参数

## 参考链接

- [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- [ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html)
- [Valgrind Manual](https://valgrind.org/docs/manual/manual.html)
- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
- [libFuzzer](https://llvm.org/docs/LibFuzzer.html)
