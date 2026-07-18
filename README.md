<h1 align="center">FastQTools</h1>

<p align="center">
  <b>一个用 C++23 写的 FASTQ 质控小工具</b><br>
  只做统计（stat）和过滤/修剪（filter）两件事，把核心路径做得快一点、稳一点。
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

## FastQTools 是做什么的

日常测序质控里，最常用的就是两个操作：

- **stat**：读入 FASTQ，输出读段数、长度分布、碱基组成、GC 含量、Q20/Q30 等统计指标。
- **filter**：一趟扫描完成长度过滤、质量过滤、N 比例过滤、质量修剪、adapter 修剪、polyG/polyX 尾修剪。

## 快速开始

```bash
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build
./build/clang-release/FastQTools --help
```

生成统计报告：

```bash
./build/clang-release/FastQTools stat \
  -i sample.fastq.gz \
  -o sample.stats.txt
```

过滤并修剪读段：

```bash
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

环境配置、安装说明和更多命令参数分别见 [快速开始指南](./docs/getting-started.md) 和 [CLI 参考](./docs/cli-reference.md)。

## 设计思路

FastQTools 不是“瑞士军刀”式工具，而是一个聚焦、可嵌入的质控组件。核心设计很直接：

- **零拷贝记录视图**：`FastqRecord` 由几个 `std::string_view` 指向同一批缓冲区，解析只做指针运算，不额外分配字符串。
- **有界流水线**：默认用 oneTBB 的三段并行流水线，I/O 阶段保序，中间计算并行；内存峰值由 `maxLiveTokens` 限制。
- **小 API 面**：公共头只有 `<fqtools/fq.h>`，CLI 和库共享同一套实现。
- **写后原子重命名**：默认先写同目录临时文件，`finish()` 成功后原子重命名；失败不会留下半成品输出。

更完整的设计说明见 [架构文档](./docs/architecture.md)。

## 什么时候用

适合：

- 需要在 C++ 项目里嵌入一个轻量 FASTQ 质控模块。
- 想要一个可控、可审计的 stat/filter 组件。
- 对 C++23 流式流水线实现感兴趣，想参考具体代码。

不适合：

- 需要全自动 adapter 识别、可视化报告、比对或变异检测。这类需求请用 fastp、fastqc 等成熟工具。

## 构建要求

- C++23 编译器：**GCC 13+** 或 **Clang 17+**
- **CMake 3.28+**
- **Conan 2.x**
- Linux / macOS；Windows 建议用 WSL 或 Docker

## 质量

CI 在 GitHub Actions 上手动触发，包含：

- 格式：clang-format
- 静态分析：clang-tidy、cppcheck
- 编译：GCC Release、Clang Release
- 动态检查：ASan、TSan、UBSan
- 测试：单元、集成、端到端
- 覆盖率：gcovr

`tools/fuzz/` 下的 fuzzer target 用于测试 FASTQ 解析器入口——唯一消费不可信外部输入的位置。

## 文档入口

| 想做的事 | 看这里 |
| --- | --- |
| 第一次构建运行 | [快速开始](./docs/getting-started.md) |
| 查询命令语法与参数 | [CLI 参考](./docs/cli-reference.md) |
| 在 C++ 项目中集成库接口 | [API 概览](./docs/api.md) |
| 了解架构与设计决策 | [架构文档](./docs/architecture.md) |
| 判断性能数字该如何解读 | [性能总览](./docs/benchmark.md) |
| 参与文档或代码贡献 | [贡献指南](./CONTRIBUTING.md) |
| 跟踪项目层面的变更 | [CHANGELOG.md](./CHANGELOG.md) |

## 许可证

FastQTools 基于 [MIT 许可证](LICENSE) 发布。

## 作者

**shijiashuai** — [GitHub](https://github.com/LessUp) · jiashuai.shi@qq.com
