# C++ CLI Development Guidelines

> Best practices for C++ CLI development in FastQTools.

---

## Overview

FastQTools 是一个使用现代 C++23 编写的高性能 FASTQ 文件处理命令行工具包，专为生物信息学质量控制（QC）工作流设计。本文档记录了实际的代码约定，供 AI 助手和团队成员参考。

---

## Pre-Development Checklist

在开始编码前，确保阅读以下文件：

- [ ] [Naming Conventions](./naming-conventions.md) — 命名规则
- [ ] [Code Style](./code-style.md) — 代码格式与风格
- [ ] [Error Handling](./error-handling.md) — 错误处理模式
- [ ] [Testing Guidelines](./testing-guidelines.md) — 测试约定

项目级规范（在 `openspec/baseline/` 中）：

- [ ] 产品规范：`openspec/baseline/product/fastq-processing.md`
- [ ] 架构规范：`openspec/baseline/architecture/`
- [ ] API 规范：`openspec/baseline/api/core-api.md`
- [ ] 测试策略：`openspec/baseline/testing/test-strategy.md`

---

## Guidelines Index

| Guide | Description | Status |
|-------|-------------|--------|
| [Directory Structure](./directory-structure.md) | 模块组织与文件布局 | Filled |
| [Naming Conventions](./naming-conventions.md) | 命名约定（类、函数、变量等） | Filled |
| [Code Style](./code-style.md) | 代码格式与风格约定 | Filled |
| [Error Handling](./error-handling.md) | 错误类型与处理策略 | Filled |
| [Memory & Performance](./memory-performance.md) | 内存管理与性能约束 | Filled |
| [Testing Guidelines](./testing-guidelines.md) | 测试组织与约定 | Filled |

---

## Tech Stack Summary

| 类别 | 技术 | 版本 | 用途 |
|------|------|------|------|
| 语言 | C++ | 23 | 现代特性、概念、范围 |
| 并行计算 | Intel oneTBB | 2022.3.0 | 流水线并行 |
| 构建系统 | CMake + Ninja | 3.28+ | 快速增量构建 |
| 包管理 | Conan | 2.x | 依赖管理 |
| 压缩 | libdeflate + zlib-ng | 1.25 / 2.3.2 | 高性能 gzip |
| 日志 | spdlog | 1.17.0 | 异步日志（仅头文件） |
| CLI | cxxopts | 3.1.1 | 参数解析 |
| 测试 | GoogleTest | 1.14+ | 单元/集成测试 |
| JSON | nlohmann_json | 3.11.3 | JSON 处理 |
| 格式化 | fmt | 12.1.0 | 格式化（仅头文件） |

---

## Key Principles

1. **零拷贝优先**：基于 `std::string_view` 的记录处理，最小化内存开销
2. **并行流水线**：使用 `tbb::parallel_pipeline`，避免热点路径串行瓶颈
3. **规范驱动开发**：所有实现以 `openspec/baseline/` 为事实来源
4. **公共 API 稳定**：`include/fqtools/` 中的头文件变更需同步更新规范

---

**Language**: 规范文档使用英文，代码注释优先使用中文。
