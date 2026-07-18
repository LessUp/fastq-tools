<h1 align="center">FastQTools</h1>

<p align="center">
  <b>C++23 FASTQ 质控工具，提供 stat 与 filter 两个命令。</b>
</p>

<p align="center">
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg" alt="CI 状态">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://img.shields.io/badge/build-GCC%20%7C%20Clang%20%7C%20ASan%20%7C%20TSan%20%7C%20UBSan-success?logo=githubactions" alt="CI 矩阵">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/releases">
    <img src="https://img.shields.io/github/v/release/LessUp/fastq-tools?label=Release&logo=github" alt="GitHub Release">
  </a>
  <a href="https://opensource.org/licenses/MIT">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License: MIT">
  </a>
  <img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23">
  <img src="https://img.shields.io/badge/CMake-3.28%2B-064F8C?logo=cmake" alt="CMake 3.28+">
  <img src="https://img.shields.io/badge/Conan-2.x-52bf50?logo=conan" alt="Conan 2.x">
  <a href="https://github.com/LessUp/fastq-tools/discussions">
    <img src="https://img.shields.io/badge/GitHub-Discussions-blue?logo=github" alt="GitHub Discussions">
  </a>
</p>

<p align="center">
  <a href="README.md">中文</a> ·
  <a href="README.en.md">English</a> ·
  <a href="./docs/getting-started.md">快速开始</a> ·
  <a href="./docs/architecture.md">架构</a> ·
  <a href="https://github.com/LessUp/fastq-tools/releases">发布版本</a>
</p>

---

## 功能

- `stat`：读段数、长度分布、碱基组成、GC 含量、Q20/Q30。
- `filter`：长度/质量/N 比例过滤；质量修剪（5'/3'/两端）；adapter 修剪；polyG/polyX 尾修剪。一趟扫描完成。

## 快速开始

```bash
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build
./build/clang-release/FastQTools --help
```

统计：

```bash
./build/clang-release/FastQTools stat \
  -i sample.fastq.gz \
  -o sample.stats.txt
```

过滤并修剪：

```bash
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

更多参数见 [CLI 参考](./docs/cli-reference.md)。

## 设计

- 零拷贝记录：`FastqRecord` 由 `std::string_view` 指向批次缓冲区，解析为指针运算。
- 有界流水线：oneTBB 三段 `parallel_pipeline`，I/O 保序，计算并行，内存峰值受 `maxLiveTokens` 限制。
- 小 API 面：公共头 `<fqtools/fq.h>`，CLI 与库共用同一实现。
- 原子输出：先写同目录临时文件，成功后重命名；失败不污染旧输出。

完整说明见 [架构文档](./docs/architecture.md)。

## 使用场景

适合：

- 在 C++ 项目中嵌入轻量 FASTQ 质控模块。
- 需要一个可控、可审计的 stat/filter 组件。
- 参考 C++23 流式流水线实现。

不适合：全自动 adapter 识别、可视化报告、比对、变异检测。这些请用 fastp、fastqc 等成熟工具。

## 构建要求

- C++23 编译器：**GCC 13+** 或 **Clang 17+**
- **CMake 3.28+**
- **Conan 2.x**
- Linux / macOS；Windows 建议 WSL 或 Docker

## 质量

CI 手动触发，包含：

- 格式：clang-format
- 静态分析：clang-tidy、cppcheck
- 编译：GCC Release、Clang Release
- 动态检查：ASan、TSan、UBSan
- 测试：单元、集成、端到端
- 覆盖率：gcovr

`tools/fuzz/` 下的 fuzzer target 针对 FASTQ 解析器入口。

## 文档

| 想做的事 | 看这里 |
| --- | --- |
| 构建与首次运行 | [快速开始](./docs/getting-started.md) |
| 命令参数 | [CLI 参考](./docs/cli-reference.md) |
| 嵌入 C++ 项目 | [API 概览](./docs/api.md) |
| 架构与设计 | [架构文档](./docs/architecture.md) |
| 性能数据解读 | [性能总览](./docs/benchmark.md) |
| 贡献 | [贡献指南](./CONTRIBUTING.md) |
| 变更记录 | [CHANGELOG.md](./CHANGELOG.md) |

## 许可证

[MIT License](LICENSE)。

## 作者

**shijiashuai** — [GitHub](https://github.com/LessUp) · jiashuai.shi@qq.com
