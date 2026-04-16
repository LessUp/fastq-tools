<h1 align="center">FastQTools</h1>

<p align="center">
  <b>现代化 FASTQ 文件处理工具集 — 高性能生物信息学质控工具</b><br>
  <b>Modern FASTQ File Processing Toolkit — High-Performance Bioinformatics QC Tool</b>
</p>

<p align="center">
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg" alt="CI">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml/badge.svg" alt="Docs">
  </a>
  <a href="https://lessup.github.io/fastq-tools/">
    <img src="https://img.shields.io/badge/docs-GitHub%20Pages-blue?logo=github" alt="Documentation">
  </a>
  <a href="https://opensource.org/licenses/MIT">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License: MIT">
  </a>
  <img src="https://img.shields.io/badge/C++-23-blue.svg" alt="C++23">
  <img src="https://img.shields.io/badge/version-3.1.0-green.svg" alt="Version">
</p>

<p align="center">
  <a href="README.md">简体中文</a> | <a href="README.en.md">English</a>
</p>

<p align="center">
  <a href="https://lessup.github.io/fastq-tools/" style="font-size: 1.1em;">
    📚 <b>在线文档 / Online Documentation</b>
  </a>
</p>

---

## ✨ 核心功能 / Core Features

<table>
<tr>
<td width="50%">

### 📊 `stat` — 统计分析

FASTQ 文件全面统计分析，输出：
- 读段总数、长度分布
- 碱基组成（A/T/C/G/N 比例）
- GC 含量、Q20/Q30 百分比

```bash
FastQTools stat -i input.fastq.gz -o report.txt
```

</td>
<td width="50%">

### 🔍 `filter` — 过滤与修剪

读段过滤与质量修剪：
- 质量阈值过滤
- 长度范围筛选
- N 碱基比例控制
- 质量修剪（两端/5'/3'端）

```bash
FastQTools filter -i input.fq.gz -o output.fq.gz \
  --min-quality 20 --min-length 50
```

</td>
</tr>
</table>

---

## 🚀 快速开始 / Quick Start

```bash
# 1. 克隆仓库 / Clone repository
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools

# 2. 安装依赖 / Install dependencies
./scripts/core/install-deps

# 3. 一键构建 / One-command build
./scripts/core/build

# 4. 查看帮助 / Show help
./build/clang-release/FastQTools --help

# 5. 运行测试 / Run tests
./scripts/core/test
```

---

## 📊 性能指标 / Performance

基于 100K reads (150bp) 的基准测试 / Benchmark with 100K reads (150bp):

| 操作 / Operation | 吞吐量 / Throughput | 性能等级 / Performance |
|-----------------|--------------------|-----------------------|
| FastQReader | 1696 MB/s | 🟢 极速 / Extreme |
| FastQWriter | 1.76 M reads/s | 🟢 极速 / Extreme |
| Filter Combined | 1.67 M reads/s | 🟢 极速 / Extreme |
| Stat Full | 302 MB/s | 🟡 高速 / High |

> 测试环境 / Test Env: AMD Ryzen 9 5900X, 32GB DDR4, NVMe SSD

---

## 🛠️ 技术栈 / Tech Stack

| 类别 / Category | 技术 / Technology | 版本 / Version |
|----------------|-------------------|----------------|
| **语言 / Language** | C++ | 23 |
| **并行计算 / Parallel** | Intel oneTBB | 2022.3.0 |
| **构建系统 / Build** | CMake | 3.28+ |
| **包管理 / Package** | Conan | 2.x |
| **压缩 / Compression** | libdeflate/zlib-ng | 1.25/2.3.2 |
| **日志 / Logging** | spdlog | 1.17.0 |
| **CLI 解析 / CLI** | cxxopts | 3.1.1 |

---

## 📁 项目结构 / Project Structure

```
fastq-tools/
├── include/fqtools/     # 公共 API 头文件 / Public API headers
├── src/                 # 源代码实现 / Source implementation
│   ├── cli/             # 命令行入口 / CLI entry
│   ├── io/              # FASTQ I/O / FASTQ I/O
│   ├── processing/      # 处理流水线 / Processing pipeline
│   └── statistics/      # 统计计算 / Statistics
├── tests/               # 测试 / Tests (unit / integration / e2e)
├── docs/                # 文档 / Documentation (中英双语 / Bilingual)
├── changelog/           # 变更记录 / Change records
├── scripts/             # 构建脚本 / Build scripts
├── docker/              # Docker 配置 / Docker configs
└── tools/               # 开发工具 / Dev tools (benchmark / fuzz)
```

---

## 📖 文档导航 / Documentation

| 文档 / Document | 中文 | English |
|----------------|------|---------|
| 快速入门 / Getting Started | [docs/guide/getting-started.md](docs/guide/getting-started.md) | [docs/guide/getting-started.en.md](docs/guide/getting-started.en.md) |
| CLI 参考 / CLI Reference | [docs/guide/cli-reference.md](docs/guide/cli-reference.md) | [docs/guide/cli-reference.en.md](docs/guide/cli-reference.en.md) |
| 架构设计 / Architecture | [docs/dev/architecture.md](docs/dev/architecture.md) | [docs/dev/architecture.en.md](docs/dev/architecture.en.md) |
| API 参考 / API Reference | [docs/api/overview.md](docs/api/overview.md) | [docs/api/overview.en.md](docs/api/overview.en.md) |

**完整文档 / Full Docs**: [https://lessup.github.io/fastq-tools/](https://lessup.github.io/fastq-tools/)

---

## 🤝 贡献 / Contributing

欢迎提交 Issue 和 Pull Request！
欢迎提交 Issue 和 Pull Request！

请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解贡献指南。
See [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines.

---

## 📜 许可证 / License

[MIT License](LICENSE) © 2025-2026 LessUp

---

<p align="center">
  <sub>Built with ❤️ by the LessUp team</sub>
</p>
