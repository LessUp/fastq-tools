# FastQTools

[![CI](https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml)
[![Deploy Docs](https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml/badge.svg)](https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml)
[![Documentation](https://img.shields.io/badge/docs-GitHub%20Pages-blue?logo=github)](https://lessup.github.io/fastq-tools/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![C++](https://img.shields.io/badge/C++-23-blue.svg)

English | [简体中文](README.zh-CN.md)

> **在线文档**: [https://lessup.github.io/fastq-tools/](https://lessup.github.io/fastq-tools/)

**FastQTools** 是一个现代化的 C++23 FASTQ 文件处理工具集，专为生物信息学高通量测序数据质控设计。

## 核心功能

| 命令 | 功能 | 示例 |
|------|------|------|
| `stat` | FASTQ 文件统计分析 | `FastQTools stat -i input.fq.gz -o report.txt` |
| `filter` | 读段过滤与质量修剪 | `FastQTools filter -i input.fq -o output.fq --min-quality 20` |

## 技术特性

- **高性能** — 基于 Intel TBB 的 `parallel_pipeline` 并行流水线处理
- **现代 C++** — C++23 标准，CMake 4.0+，Conan 2.x 依赖管理
- **零拷贝 I/O** — `FastqRecord` 使用 `string_view` 实现高效内存访问
- **全面质控** — Sanitizers、Valgrind、模糊测试、覆盖率全覆盖
- **模块化设计** — 清晰的接口-实现分离，支持库级别集成

## 快速开始

```bash
# 克隆仓库
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools

# 一键构建（需要 Conan）
./scripts/core/build

# 查看帮助
./build/gcc-release/FastQTools --help

# 统计分析
./build/gcc-release/FastQTools stat -i input.fastq.gz -o output.stat.txt

# 过滤处理
./build/gcc-release/FastQTools filter -i input.fq.gz -o filtered.fq.gz \
    --min-quality 20 --min-length 50
```

## 依赖

| 依赖 | 版本 | 用途 |
|------|------|------|
| Intel oneTBB | 2022.3.0 | 并行流水线 |
| zlib-ng | 2.3.2 | gzip 压缩/解压 |
| libdeflate | 1.25 | 高性能 deflate |
| cxxopts | 3.1.1 | 命令行解析 |
| spdlog | 1.17.0 | 日志框架 |
| fmt | 12.1.0 | 格式化库 |
| nlohmann_json | 3.11.3 | JSON 处理 |

## 项目结构

```
fastq-tools/
├── include/fqtools/     # 公共 API 头文件
├── src/                 # 源代码实现
│   ├── cli/             # 命令行入口
│   ├── io/              # FASTQ I/O
│   ├── processing/      # 处理流水线
│   └── statistics/      # 统计计算
├── tests/               # 测试（unit / integration / e2e）
├── config/              # 构建配置
├── scripts/             # 构建与工具脚本
├── docker/              # Docker 配置
├── tools/               # 开发工具（benchmark / fuzz）
├── docs/                # 项目文档
└── changelog/           # 变更记录
```

## 构建

```bash
# 安装 Conan（如未安装）
pip install conan==2.24.0
conan profile detect --force

# 构建
./scripts/core/build --compiler gcc --type Release

# 运行测试
./scripts/core/test --build-dir build/gcc-release
```

## 开发环境

推荐使用 DevContainer 进行开发：

```bash
# VS Code: Ctrl+Shift+P → "Reopen in Container"
# 或使用 Docker Compose
docker compose -f docker/docker-compose.yml up dev
```

## 性能

基于 100K reads (150bp) 的基准测试：

| 操作 | 吞吐量 |
|------|--------|
| FastQReader | 1696 MB/s |
| FastQWriter | 1.76 M reads/s |
| Filter Combined | 1.67 M reads/s |

## 文档

- [用户指南](docs/guide/getting-started.md) — 快速入门
- [API 参考](docs/api/overview.md) — 编程接口
- [开发者指南](docs/dev/architecture.md) — 架构设计

## 许可证

[MIT License](LICENSE)

## 贡献

欢迎提交 Issue 和 Pull Request。请参阅 [贡献指南](CONTRIBUTING.md)。
