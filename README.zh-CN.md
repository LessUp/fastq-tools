<h1 align="center">FastQTools</h1>

<p align="center">
  <b>专注的现代 C++23 FASTQ 质控工具集</b><br>
  <i>零拷贝记录视图、可替换流式 backend、最小可嵌入 API 接口——把少数 QC 任务做到极致。</i>
</p>

<p align="center">
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg" alt="CI 状态">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://img.shields.io/badge/build-GCC%20%7C%20Clang%20%7C%20ASan%20%7C%20TSan%20%7C%20UBSan-success?logo=githubactions" alt="CI 矩阵">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/releases">
    <img src="https://img.shields.io/github/v/release/LessUp/fastq-tools?label=Release&logo=github" alt="GitHub 发布版本">
  </a>
  <a href="https://opensource.org/licenses/MIT">
    <img src="https://img.shields.io/badge/许可证-MIT-yellow.svg" alt="License: MIT">
  </a>
  <img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23">
  <img src="https://img.shields.io/badge/CMake-3.28%2B-064F8C?logo=cmake" alt="CMake 3.28+">
  <img src="https://img.shields.io/badge/Conan-2.x-52bf50?logo=conan" alt="Conan 2.x">
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

## 目录

- [为何做 FastQTools](#为何做-fastqtools)
- [定位与同类工具](#定位与同类工具)
- [核心能力](#核心能力)
- [快速开始](#快速开始)
- [可嵌入 C++ API](#可嵌入-c-api)
- [架构一览](#架构一览)
- [代表性性能](#代表性性能)
- [质量门槛](#质量门槛)
- [文档入口](#文档入口)
- [构建要求](#构建要求)
- [贡献](#贡献)
- [许可证](#许可证)
- [作者](#作者)

---

## 为何做 FastQTools

多数 FASTQ 质控工具追求广度：功能多、报告格式多、边界情况多。FastQTools 反其道而行——一个 **C++23 工程能力展示项目**，刻意把维护面收得很窄（`stat`、`filter`、一个总入口头文件），把工程预算投到那些通常看不见的地方：

- 零拷贝记录模型，解析退化为指针算术，不分配字符串。
- 私有执行 backend seam，默认 oneTBB 流水线天然保序。
- 批量 + 对象池内存纪律，热点路径不逐条分配。
- CI 矩阵通过 GitHub Actions 页面手动触发，运行 GCC、Clang、ASan、TSan、UBSan，外加解析器入口的 fuzzer target（CI 集成待定）。

核心主张很简单：**把少数事做到极致，比把很多事做到及格更难**。本仓库就是这一主张的实证。

## 定位与同类工具

FastQTools 聚焦测序日常 QC 的两个最高频操作——汇总统计与读段过滤/修剪——以及一个最小可嵌入 C++ API，用于把它们接进更大的流水线。它刻意**不做**比对、变异检测、组装或可视化。

| 工具 | 范围 | 语言 | 强项 | FastQTools 与之的差异 |
| --- | --- | --- | --- | --- |
| **fastp** | 全能 QC + adapter/修剪 + 报告 | C++11 | 成熟、功能全、事实标准 | 维护面更窄；现代 C++23 内核；可嵌入 API；更严格的 CI/消毒剂 |
| **fastqc** | 仅 QC 报告（Java） | Java | 视觉报告丰富、采用广 | 原生 CLI；可组合；无 JVM；C++ 可嵌入 |
| **seqkit** | 全能 SEQ 工具箱（Go） | Go | 算子面巨大、便于脚本化 | 聚焦 QC 热点路径；原生 C++ 吞吐；二进制更小 |
| **FastQTools** | `stat` + `filter` + 可嵌入 API | C++23 | 零拷贝 + TBB 流式 + 消毒剂加固 | — |

FastQTools 不是 fastp 的直接替代品。它是一个聚焦、现代内核的替代方案——当你想要一个小巧、可审计、可嵌入的 QC 组件，或一份 C++23 流式流水线的参考实现时，它正合适。

## 核心能力

- **`stat` — FASTQ 统计。** 读段数、长度分布、碱基组成、GC 含量、Q20/Q30 等质量指标，可选签名 sidecar 报告（旁路报告，含 head k-mer、重复估计）。
- **`filter` — 单遍过滤与修剪。** 长度、平均质量、N 比例阈值；质量修剪（5'/3'/两端）；adapter 修剪；polyG / polyX 尾修剪——全部在一次流式扫描中完成。
- **可嵌入 C++ API。** 单一总入口头 `<fqtools/fq.h>`；CLI 与库共用同一条流水线，行为一致。
- **零拷贝记录视图。** `FastqRecord` 是五个 `std::string_view`，指向连续批缓冲区；解析是指针算术，不是分配。
- **可替换流式 backend。** 默认 oneTBB 使用 `serial_in_order → parallel → serial_in_order`；I/O 保序、归约确定，CPU 密集的中间级跨核扩展。
- **消毒剂加固的 CI。** 手动触发后运行 GCC Release、Clang Release、Clang ASan/TSan/UBSan、clang-tidy、cppcheck；fuzzer target 位于 `tools/fuzz/`（CI 集成待定）。

## 快速开始

```bash
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build
./build/clang-release/FastQTools --help
```

生成一份 QC 报告：

```bash
./build/clang-release/FastQTools stat \
  -i sample.fastq.gz \
  -o sample.stats.txt
```

单遍过滤并修剪读段：

```bash
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

环境配置、安装说明与部署入口见 [快速开始指南](./docs/getting-started.md)。完整参数见 [CLI 参考](./docs/cli-reference.md)。

## 可嵌入 C++ API

CLI 与库共用同一条流水线。嵌入即可获得相同行为，无需子进程。

```cpp
#include <fqtools/fq.h>

// 统计
fq::statistics::StatisticOptions options;
options.inputFastqPath = "sample.fastq.gz";
options.outputStatPath  = "sample.stats.txt";
options.processing.batchSize   = 50000;
options.processing.threadCount = 8;

fq::statistics::Calculator calculator(std::move(options));
calculator.run();
```

```cpp
#include <fqtools/fq.h>

// 过滤 + 修剪，通过谓词与修改器组合
fq::processing::Pipeline pipeline;
pipeline.setInputPath("sample.fastq.gz");
pipeline.setOutputPath("sample.filtered.fastq.gz");

pipeline.addReadPredicate(std::make_unique<fq::processing::MinQualityPredicate>(20.0));
pipeline.addReadMutator(std::make_unique<fq::processing::QualityTrimmer>(
    20.0, 1, fq::processing::QualityTrimmer::TrimMode::Both, 33));

fq::processing::ProcessingOptions opts;
opts.batchSize   = 50000;
opts.threadCount = 8;
pipeline.setProcessingOptions(opts);

auto stats = pipeline.run();
```

`Pipeline` 是 move-only 具体类，并通过 PIMPL 隐藏实现；`setReader` / `setWriter` 接受 `unique_ptr<IReader>` / `IWriter`，测试可注入 mock。完整头文件映射见 [API 概览](./docs/api.md)。

## 架构一览

```
                 ┌─────────── src/cli ────────────┐
                 │  main → CommandRegistry         │
                 │   ├─ StatCommand                │
                 │   └─ FilterCommand              │
                 └────────┬────────────────────────┘
                          │ 组装 options + 注入依赖
          ┌───────────────┼───────────────┐
          ▼               ▼               ▼
   src/io            src/processing    src/statistics
   Reader/Writer     Pipeline          Calculator/Writer
   (gzip, 批量)      (backend, 谓词)   (Q20/Q30, GC, 长度)
          │               │               │
          └───────────────┼───────────────┘
                          ▼
                   include/fqtools/  ← 公共 Façade (fq.h)
```

热点路径通过私有 `ExecutionBackend` seam 执行；默认 oneTBB 路径是一条三级 `parallel_pipeline`：

```
serial_in_order (读取批次)  →  parallel (过滤/修剪/统计)  →  serial_in_order (写出 + 归约)
```

- **第一级 — 串行保序。** gzip 流是顺序格式，读段按文件顺序产出。
- **第二级 — 并行。** CPU 密集工作（谓词、修改器、逐碱基统计）跨核扩展，批次之间相互独立。
- **第三级 — 串行保序。** 输出 FASTQ 保序；统计按批次确定性归约。

在途批数受 `maxLiveTokens` 限制，内存峰值可预测。批次来自 `ObjectPool`，热点路径无逐批分配开销。

调度框架不暴露给公共 API：默认 oneTBB，另有串行基线和默认关闭的 Taskflow 实验 backend。三者使用相同 I/O、批处理操作与计量契约，便于公平基准和后续演进。

三个值得说明的设计决策：

1. **`FastqRecord` 全是 `string_view`。** 生命周期绑定到所属 `FastqBatch`，记录不能逃逸出批次。这是刻意约束，保证零拷贝的安全性。
2. **`FastqBatch` 持有一块连续缓冲区。** `clear()` 保留容量并复用，下一批直接覆盖——无 `malloc/free` 开销。
3. **每个领域一个 `interfaces.h`。** 不搞细粒度接口蔓延；依赖注入仍可用于测试。

完整设计缘由（不只是"是什么"）见 [架构文档](./docs/architecture.md)。

## 代表性性能

**100K reads（150 bp）**、**AMD Ryzen 9 5900X**、Clang Release 下的点时快照（持续维护）。适合粗略判断量级，不是所有数据集、压缩级别或存储环境下的绝对承诺。

| 工作负载 | 代表性结果 |
| --- | --- |
| FASTQ 读取路径 | 1696 MB/s |
| FASTQ 写出路径 | 176 万 reads/s |
| 组合过滤处理 | 167 万 reads/s |
| 完整统计分析 | 302 MB/s |

**如何解读这些数字。**

- 读取路径接近 1.7 GB/s——瓶颈在 gzip 解压和磁盘，不在解析。这正是零拷贝 + 批量 + 连续内存设计的体现。
- 写出路径主要由 gzip 压缩主导；批量写入分摊系统调用开销。
- 组合过滤接近纯写出吞吐，说明过滤/修剪的 CPU 开销很小。
- 统计较低，因为逐碱基质量、GC 滑窗、长度直方图是 CPU 密集的，且没有写出路径可分摊。

**对比背景。** 与 fastp/seqkit 的直接同环境对比数字高度依赖环境与参数，因此未列入本表。`tools/benchmark/` 中的基准套件（Google Benchmark）可复现；要在你的硬件上跑并补一行对比，见 [性能总览](./docs/benchmark.md)。

## 质量门槛

CI（`.github/workflows/ci.yml`）通过 GitHub Actions 页面手动触发；每次运行执行：

| 任务 | 检测内容 |
| --- | --- |
| **格式** | clang-format 漂移 |
| **静态分析** | clang-tidy + cppcheck：API 误用、现代 C++ 反模式 |
| **GCC Release** | 跨编译器可移植性 |
| **Clang Release** | 主生产工具链 |
| **Clang ASan** | 越界、UAF、泄漏 |
| **Clang TSan** | TBB 流水线中的数据竞争 |
| **Clang UBSan** | 未定义行为 |
| **覆盖率** | gcovr 行覆盖率 + Codecov 上传 |

fuzzer target 位于 `tools/fuzz/`，针对 FASTQ 解析器入口——唯一消费不可信外部输入的位置。它们尚未接入 CI；可用 libFuzzer 构建在本地运行。测试分层：单元（镜像 `src/`）、集成（跨模块）、端到端（CLI 黑盒，Bash + Python）。

## 文档入口

| 如果你想… | 建议从这里开始 |
| --- | --- |
| 完成构建并跑通第一个命令 | [快速开始](./docs/getting-started.md) |
| 查询命令语法与参数 | [CLI 参考](./docs/cli-reference.md) |
| 在 C++ 项目中集成库接口 | [API 概览](./docs/api.md) |
| 了解架构与设计决策 | [架构文档](./docs/architecture.md) |
| 判断性能数字该如何解读 | [性能总览](./docs/benchmark.md) |
| 查阅性能数据与优化历程 | [性能数据与优化记录](./docs/performance/README.md) |
| 复盘问题定位与解决方案 | [问题复盘库](./docs/postmortems/README.md) |
| 参与文档或代码贡献 | [贡献指南](./CONTRIBUTING.md) |
| 跟踪项目层面的变更 | [CHANGELOG.md](./CHANGELOG.md) |

## 构建要求

- 支持 C++23 的编译器（最低 **GCC 13+** 或 **Clang 17+**；CI 跑 GCC 13——Ubuntu runner 默认——与 Clang 21）
- **CMake 3.28+**
- **Conan 2.x**
- 当前支持 Linux 与 macOS；Windows 用 Docker 或 WSL。

## 贡献

欢迎提交范围明确的改进：bug 报告、文档修订、测试补充、基准工作，以及边界清晰的功能调整。

- 阅读 [贡献指南](./CONTRIBUTING.md)
- 通过 [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) 反馈问题
- 在 [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions) 讨论想法

## 许可证

FastQTools 基于 [MIT 许可证](LICENSE) 发布。技术叙事——每个决策的"为什么"——见 [docs/architecture.md](./docs/architecture.md)。

## 作者

**shijiashuai** — [GitHub](https://github.com/LessUp) · jiashuai.shi@qq.com
