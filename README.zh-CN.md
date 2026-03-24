# FastQTools

[![CI](https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml)
[![Deploy Docs](https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml/badge.svg)](https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml)
[![Documentation](https://img.shields.io/badge/docs-GitHub%20Pages-blue?logo=github)](https://lessup.github.io/fastq-tools/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![C++](https://img.shields.io/badge/C++-23-blue.svg)

[English](README.md) | 简体中文

> **在线文档**: [https://lessup.github.io/fastq-tools/](https://lessup.github.io/fastq-tools/)

一个面向 FASTQ 文件的现代 C++ 高性能处理工具集，专注于生物信息学场景中的质控、过滤和统计分析。

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

## 核心功能

- **`stat` 命令**: FASTQ 文件统计分析，输出碱基/质量分布等统计结果
- **`filter` 命令**: FASTQ 读段过滤与剪切，支持质量、长度、N 比例等条件

## 技术特性

- **高性能**: 基于 Intel TBB 的并行流水线处理
- **现代化**: C++23 标准 + 现代 CMake 构建系统
- **模块化**: 清晰的接口设计，支持库级别集成

## 依赖清单

- **并发**: Intel oneTBB（流水线核心依赖）
- **压缩**: zlib-ng / libdeflate
- **CLI & 日志**: cxxopts、spdlog、fmt、nlohmann-json
- **包管理**: Conan（默认）

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
├── docs/              # 项目文档（部署、迁移、开发指南等）
├── changelog/         # 变更记录（每条独立 .md 文件）
└── examples/          # 使用示例
```

### 配置目录 (config/)

包含所有构建与分析配置，按用途组织：
- `conan/` - Conan 编译器 profiles（profile-clang, profile-gcc）
- `dependencies/` - Conan 依赖配置（conanfile.py）
- `sanitizers/` - ASan/TSan/UBSan/MSan 运行时选项
- `valgrind/` - Valgrind 抑制规则
- `cppcheck/` - Cppcheck 静态分析配置
- `coverage/` - 覆盖率阈值配置
- `iwyu/` - Include-What-You-Use 映射

### 工具目录 (tools/)

包含开发与测试相关的工具：
- `benchmark/` - 性能基准测试（Google Benchmark）
- `fuzz/` - 模糊测试
- `data/` - 测试用 FASTQ 数据

## 构建与运行

### 环境要求
- CMake ≥ 3.20
- 支持 C++23 的编译器（最低 GCC 11+ / Clang 12+，推荐 GCC 15；Clang 开发构建可由脚本按本机版本自动适配 Conan）
- Intel oneTBB 运行时（并发库）
- Ninja（推荐）
- Conan（依赖管理）

### 快速构建
```bash
# 一键构建 (Clang + Release)
./scripts/core/build

# 指定配置
./scripts/core/build --compiler gcc --type Debug

# 启用 Sanitizers
./scripts/core/build --sanitizer asan
./scripts/core/build --sanitizer tsan

# 覆盖率构建
./scripts/core/build --coverage
```

## 开发指南

- 代码与命名: 见 [编码规范](docs/dev/coding-standards.md)
- 提交信息: 见 [Git 提交规范](docs/dev/git-guidelines.md)
- Dev Container: 见 [DevContainer 开发环境](docs/dev/devcontainer.md)
- 脚本说明: 见 [scripts/README.md](scripts/README.md)

## 文档

- [构建指南](docs/dev/build.md) - 系统要求和构建说明
- [CLI 参考](docs/guide/cli-reference.md) - 命令行用法和示例
- [架构设计](docs/dev/architecture.md) - 技术架构和设计原则
- [编码规范](docs/dev/coding-standards.md) - 代码与文件命名约定
- [Git 提交规范](docs/dev/git-guidelines.md) - 提交格式与示例
- [性能报告](docs/dev/benchmark-guide.md) - 性能基准测试与报告
 
 <!-- BENCHMARK_START -->
 ## 性能
 
 FastQTools 针对高吞吐量场景进行了优化。运行 `./scripts/benchmark run` 获取最新性能数据。
 
 ### 最新基准测试结果（100K reads, 150bp）
 
 | Operation | Throughput | Time |
 |-----------|------------|------|
 | FastQReader Medium | 1696.50 MB/s | 18.8 ms |
 | FastQWriter Medium | 1.76 M reads/s | 57.0 ms |
 | Filter Combined (100K) | 1.67 M reads/s | 60.5 ms |
 | Stat Full (100K) | 301.91 MB/s | 104.6 ms |
 
 - **详细报告**: [docs/benchmark-reports/latest.md](docs/benchmark-reports/latest.md)
 - **原始数据**: [docs/benchmark-reports/latest.json](docs/benchmark-reports/latest.json)
 
 ```bash
 # 运行基准测试
 ./scripts/benchmark run
 
 # 生成性能报告
 ./scripts/benchmark report
 
 # 检测性能回归
 ./scripts/benchmark compare baseline.json current.json
 ```
 <!-- BENCHMARK_END -->
 
 ## 许可证

MIT License - 详见 [LICENSE](LICENSE) 文件
