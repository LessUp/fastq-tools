<h1 align="center">FastQTools</h1>

<p align="center">
  <b>聚焦 FASTQ 质控的工具集</b><br>
  <i>面向测序数据的快速统计、过滤与修剪；既可直接命令行使用，也可嵌入 C++ 工具链。</i>
</p>

<p align="center">
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg" alt="CI 状态">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/releases">
    <img src="https://img.shields.io/github/v/release/LessUp/fastq-tools?label=Release&logo=github" alt="GitHub 发布版本">
  </a>
  <a href="https://opensource.org/licenses/MIT">
    <img src="https://img.shields.io/badge/许可证-MIT-yellow.svg" alt="License: MIT">
  </a>
  <img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23">
  <a href="https://github.com/LessUp/fastq-tools/discussions">
    <img src="https://img.shields.io/badge/GitHub-Discussions-blue?logo=github" alt="GitHub 讨论">
  </a>
</p>

<p align="center">
  <a href="README.md">English</a> •
  <a href="README.zh-CN.md">简体中文</a> •
  <a href="./docs/architecture.md">架构</a> •
  <a href="https://github.com/LessUp/fastq-tools/releases">发布版本</a>
</p>

---

FastQTools 是一个基于现代 C++23 的 FASTQ 质控工具集，适合希望快速检查、过滤、修剪 reads 的研究人员和流水线工程师。当前维护面刻意保持精简：`stat` 命令、`filter` 命令，以及一个以总入口头文件 `fqtools/fq.h` 为中心的最小可嵌入 C++ API。

## 它最适合做什么

- **FASTQ 统计 (`stat`)**：读段数量、长度分布、碱基组成、GC 含量、Q20/Q30 等质量指标。
- **过滤与修剪 (`filter`)**：长度、质量、N 比例过滤，加上低质量端修剪，同一条处理链完成。
- **可嵌入的 C++ API（`fqtools/fq.h`）**：单一总入口头文件，与 CLI 对齐。
- **性能导向实现**：零拷贝记录视图、Intel oneTBB 并行流水线、压缩文件工作流支持。

## 快速开始

```bash
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build
./build/clang-release/FastQTools --help

# 生成一份 QC 统计报告
./build/clang-release/FastQTools stat \
  -i sample.fastq.gz \
  -o sample.stats.txt

# 过滤并修剪 reads
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

如果你需要环境配置、安装说明或部署入口，建议先看[快速开始指南](./docs/getting-started.md)。

## 代表性性能数据

下表是 **100K reads（150 bp）**、**AMD Ryzen 9 5900X** 环境下维护中的基准快照。它们适合帮助你判断量级，不应被理解为所有数据集、压缩级别或存储环境下的绝对承诺。

| 工作负载 | 代表性结果 |
| --- | --- |
| FASTQ 读取路径 | 1696 MB/s |
| FASTQ 写出路径 | 176 万 reads/s |
| 组合过滤处理 | 167 万 reads/s |
| 完整统计分析 | 302 MB/s |

更完整的背景说明见[性能总览](./docs/benchmark.md)。

## 文档入口

| 如果你想… | 建议从这里开始 |
| --- | --- |
| 完成构建并跑通第一个命令 | [快速开始](./docs/getting-started.md) |
| 查询命令语法与参数 | [CLI 参考](./docs/cli-reference.md) |
| 在 C++ 项目中集成库接口 | [API 概览](./docs/api.md) |
| 了解架构与设计决策 | [架构文档](./docs/architecture.md) |
| 判断性能数字该如何解读 | [性能总览](./docs/benchmark.md) |
| 参与文档或代码贡献 | [贡献指南](./CONTRIBUTING.md) |
| 跟踪项目层面的变更 | [CHANGELOG.md](./CHANGELOG.md) |

## 构建要求

- 支持 C++23 的编译器（最低 **GCC 11+** 或 **Clang 12+**）
- **CMake 3.28+**
- **Conan 2.x**
- 当前支持的构建环境包括 Linux 和 macOS；Windows 用户建议使用 Docker 或 WSL。

## 贡献

欢迎提交聚焦明确的改进：bug 报告、文档修订、测试补充、基准工作，以及范围清晰的功能调整。

- 阅读[贡献指南](./CONTRIBUTING.md)
- 通过 [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) 反馈问题
- 在 [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions) 讨论想法

## 许可证

FastQTools 基于 [MIT 许可证](LICENSE) 发布。

## 作者

**shijiashuai** — [GitHub](https://github.com/LessUp) · jiashuai.shi@qq.com

C++23 FASTQ 工具集。技术叙事见 [docs/architecture.md](./docs/architecture.md)。
