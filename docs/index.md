---
hide:
  - navigation
  - toc
---

# FastQTools

<div style="text-align: center; margin: 2rem 0;">

[![CI](https://img.shields.io/github/actions/workflow/status/LessUp/fastq-tools/ci.yml?label=CI&logo=github)](https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml)
[![Docs](https://img.shields.io/github/actions/workflow/status/LessUp/fastq-tools/pages.yml?label=Docs&logo=github)](https://lessup.github.io/fastq-tools/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![C++23](https://img.shields.io/badge/C++-23-blue.svg)

**现代化 FASTQ 文件处理工具集 — 高性能生物信息学质控工具**

</div>

---

## 核心功能

<div class="grid cards" markdown>

-   :material-chart-bar:{ .lg .middle } **统计分析 (`stat`)**

    ---

    FASTQ 文件统计分析，输出碱基组成、质量分布、GC 含量等核心指标

    ```bash
    FastQTools stat -i input.fastq.gz -o output.stat.txt
    ```

-   :material-filter:{ .lg .middle } **过滤处理 (`filter`)**

    ---

    FASTQ 读段过滤与剪切，支持质量、长度、N 比例等多维条件

    ```bash
    FastQTools filter -i input.fq.gz -o filtered.fq.gz \
        --min-quality 20 --min-length 50
    ```

</div>

## 技术特性

| 特性 | 描述 |
|------|------|
| **高性能** | 基于 Intel TBB 的 `parallel_pipeline` 并行流水线处理 |
| **现代化** | C++23 标准 + 现代 CMake 构建系统 |
| **模块化** | 清晰的接口-实现分离设计，支持库级别集成 |
| **零拷贝 I/O** | `FastqRecord` 使用 `string_view` 实现高效内存访问 |
| **全面质控** | Sanitizers、Valgrind、模糊测试、覆盖率全覆盖 |

## 快速开始

```bash
# 一键构建
./scripts/core/build

# 查看帮助
./build/clang-release/FastQTools --help

# 统计分析
FastQTools stat -i input.fastq.gz -o output.stat.txt

# 过滤处理
FastQTools filter -i input.fq.gz -o filtered.fq.gz --min-quality 20 --min-length 50
```

[:octicons-arrow-right-24: 完整入门指南](guide/getting-started.md){ .md-button .md-button--primary }
[:octicons-book-24: API 参考](api/overview.md){ .md-button }

## 依赖清单

| 依赖 | 版本 | 用途 |
|------|------|------|
| **Intel oneTBB** | 2022.3.0 | 并行流水线核心 |
| **zlib-ng** | 2.3.2 | gzip 压缩/解压 |
| **libdeflate** | 1.25 | 高性能 deflate |
| **cxxopts** | 3.1.1 | 命令行参数解析 |
| **spdlog** | 1.17.0 | 日志框架 |
| **fmt** | 12.1.0 | 格式化库 |
| **nlohmann_json** | 3.11.3 | JSON 处理 |

## 性能概览

基于 100K reads (150bp) 的基准测试结果：

| 操作 | 吞吐量 | 耗时 |
|------|--------|------|
| FastQReader | 1696 MB/s | 18.8 ms |
| FastQWriter | 1.76 M reads/s | 57.0 ms |
| Filter Combined | 1.67 M reads/s | 60.5 ms |
| Stat Full | 302 MB/s | 104.6 ms |

[:octicons-graph-24: 完整 Benchmark 报告](benchmark-reports/latest.md){ .md-button }

## 项目结构

```
FastQTools/
├── include/fqtools/   # 公共 API 头文件（对外接口）
├── src/               # 源代码实现
├── tests/             # 测试（unit / integration / e2e）
├── config/            # 构建配置（Conan profiles、sanitizers、valgrind 等）
├── scripts/           # 构建与工具脚本（core / lib / tools）
├── docker/            # Docker 配置（dev / prod / deploy）
├── tools/             # 开发工具（benchmark / fuzz / data）
├── cmake/modules/     # 自定义 CMake 模块
├── docs/              # 项目文档
├── changelog/         # 变更记录
└── examples/          # 使用示例
```

---

<div style="text-align: center; margin-top: 2rem; color: #888; font-size: 0.9rem;">
MIT License · Copyright &copy; 2025-2026 LessUp
</div>
