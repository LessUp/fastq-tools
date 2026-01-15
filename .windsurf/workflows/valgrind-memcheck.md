---
title: "/valgrind-memcheck"
description: "使用 Valgrind Memcheck 运行 FastQTools，可用于定位内存泄漏/非法访问（较慢但稳定）。"
auto_execution_mode: 1
---

## 适用场景
- ASan 不方便使用（例如环境限制）或需要更严格的 leak 检测
- 你希望对 CLI 或测试做一次 memcheck

## 工作流步骤
1. **构建（建议 Debug）**
   - `./scripts/core/build -c clang -t Debug --no-lto`

2. **对主可执行文件运行 memcheck（示例：stat）**
   - `./scripts/tools/valgrind-memcheck -- stat tools/data/sample_10k.fastq`

3. **（可选）对测试可执行文件运行 memcheck**
   - `./scripts/tools/valgrind-memcheck --test`

## 备注
- Valgrind 很慢，建议先用最小输入复现。
