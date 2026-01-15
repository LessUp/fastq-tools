---
title: "/asan-check"
description: "使用 AddressSanitizer (ASan) 构建并运行测试，定位内存越界/Use-After-Free 等问题。"
auto_execution_mode: 1
---

## 适用场景
- 崩溃/疑似内存越界、UAF、double free
- 你需要一个比 Valgrind 更快的内存错误检测手段

## 工作流步骤
1. **构建（Debug + ASan + 禁用 LTO）**
   - `./scripts/core/build -c clang -t Debug -s asan --no-lto -b build-clang-asan`

2. **运行测试**
   - `./scripts/core/test -b build-clang-asan`

## 备注
- ASan 的报错堆栈通常已经足够定位问题；必要时再配合 Valgrind memcheck。
