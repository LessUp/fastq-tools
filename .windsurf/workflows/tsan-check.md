---
title: "/tsan-check"
description: "使用 ThreadSanitizer (TSan) 构建并运行测试，定位数据竞争/竞态问题。"
auto_execution_mode: 1
---

## 适用场景
- 多线程相关改动（任务调度、队列、并发读写）
- 你怀疑存在 data race 或 lock 使用问题

## 工作流步骤
1. **构建（Debug + TSan + 禁用 LTO）**
   - `./scripts/core/build -c clang -t Debug -s tsan --no-lto -b build-clang-tsan`

2. **运行测试**
   - `./scripts/core/test -b build-clang-tsan`

## 备注
- TSan 运行会显著变慢，建议优先用 `-f "<pattern>"` 缩小测试范围。
