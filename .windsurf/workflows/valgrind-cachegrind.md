---
title: "/valgrind-cachegrind"
description: "使用 Valgrind Cachegrind 做缓存/指令级性能剖析（生成报告）。"
auto_execution_mode: 1
---

## 适用场景
- 你怀疑性能瓶颈与缓存/分支预测相关
- 需要一个 deterministic 的 profiler（但会非常慢）

## 工作流步骤
1. **构建（建议 Release 或 RelWithDebInfo）**
   - `./scripts/core/build -c clang -t RelWithDebInfo --no-lto`

2. **运行 Cachegrind（示例：stat）**
   - `./scripts/tools/valgrind-cachegrind -- stat tools/data/sample_10k.fastq`

## 产物
- Cachegrind 输出文件会写到脚本提示的位置（可用于 `cg_annotate` / `kcachegrind`）。
