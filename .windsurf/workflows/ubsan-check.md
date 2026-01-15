---
title: "/ubsan-check"
description: "使用 UndefinedBehaviorSanitizer (UBSan) 构建并运行测试，定位未定义行为。"
auto_execution_mode: 1
---

## 适用场景
- 你怀疑存在整数溢出、无效 shift、越界等 UB
- CI/本地出现仅在某些编译器/优化级别下复现的问题

## 工作流步骤
1. **构建（Debug + UBSan + 禁用 LTO）**
   - `./scripts/core/build -c clang -t Debug -s ubsan --no-lto -b build-clang-ubsan`

2. **运行测试**
   - `./scripts/core/test -b build-clang-ubsan`

## 备注
- UBSan 经常能直接指出触发 UB 的源码位置。
