# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- 代码质量优化：添加 `[[nodiscard]]` 属性、移除未使用成员变量、统一日志输出

### Changed
- 文档全面优化重构

---

## [3.1.0] - 2025-08-01

### Added

**核心功能**
- `stat` 命令：FASTQ 文件统计分析，输出碱基组成、质量分布、GC 含量等
- `filter` 命令：读段过滤与质量修剪，支持质量阈值、长度范围、N 比例等条件

**性能优化**
- 基于 Intel TBB 的 `parallel_pipeline` 并行流水线处理
- `FastqRecord` 使用 `string_view` 实现零拷贝 I/O
- libdeflate 高性能压缩/解压支持

**开发工具**
- 完整的单元测试、集成测试、端到端测试覆盖
- Google Benchmark 性能基准测试
- LibFuzzer 模糊测试支持
- ASan/TSan/UBSan Sanitizers 集成
- Valgrind 内存检查配置

**构建系统**
- CMake 4.0+ 现代 CMake 配置
- Conan 2.x 依赖管理
- GCC 15 / Clang 21 编译器支持
- CMake Presets 多配置支持

**Docker 支持**
- 开发环境镜像 (gcc:15.2-bookworm)
- 生产部署镜像 (debian:bookworm-slim)
- VS Code DevContainer 配置

### Changed

- 从 C++20 升级到 C++23 标准
- 实现接口-实现分离的模块化架构
- 统一的错误处理和日志系统
- MongoDB 风格命名规范

---

## [3.0.0] - 2025-07-31

### Added
- 初始核心功能实现
- CMake 构建系统
- Conan 依赖管理

### Changed
- 从 C++20 模块回归传统头文件结构
- 模块化代码架构

---

## [2.0.0] - 2024-07-29

### Added
- 初始版本发布
- FastQ 文件统计分析功能
- 多线程处理支持
- 压缩文件格式支持
- 命令行界面

---

## 变更记录

- **详细变更**：见 [`changelog/`](changelog/) 目录
- **年度汇总**：见 [`changelog/2025-summary.md`](changelog/2025-summary.md)、[`changelog/2026-summary.md`](changelog/2026-summary.md)
