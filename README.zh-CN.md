<h1 align="center">FastQTools</h1>

<p align="center">
  <b>高性能 FASTQ 文件处理工具集</b><br>
  <i>基于现代 C++23 的生物信息学质控工具，支持零拷贝 I/O 和并行处理</i>
</p>

<p align="center">
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg" alt="CI 状态">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml/badge.svg" alt="文档状态">
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
  <a href="https://lessup.github.io/fastq-tools/">在线文档</a> •
  <a href="https://github.com/LessUp/fastq-tools/releases">发布版本</a>
</p>

---

## 🌟 概述

FastQTools 是一个基于 **现代 C++23** 的**高性能 FASTQ 文件（测序数据）处理工具包**，提供：

- ⚡ **极致性能**: 基于 TBB 并行流水线，高达 **170万 reads/秒** 的处理速度
- 🚀 **零拷贝 I/O**: 基于 `std::string_view` 的记录处理，最小内存开销
- 🔧 **生产就绪**: 全面的消毒剂、模糊测试和 CI/CD 验证
- 📦 **开发者友好**: CMake + Conan 构建系统，清晰的 API 和 Docker 支持

适用于生物信息学流水线、NGS 数据处理和基因组质控工作流。

---

## ✨ 核心功能

### 📊 统计分析 (`stat`)

一条命令即可完成全面的 FASTQ 文件统计：

- **读段指标**: 总数、长度分布、平均值/中位数/最小值/最大值
- **碱基组成**: 每个位置的 A/T/C/G/N 比例
- **质量分析**: Q20/Q30 百分比、质量分数分布
- **GC 含量**: GC 百分比和分布曲线
- **输出格式**: 人类可读文本、JSON 格式供程序调用

```bash
FastQTools stat -i input.fastq.gz -o report.txt
FastQTools stat -i input.fastq.gz -o report.json --format json
```

### 🔍 过滤与质量修剪 (`filter`)

灵活的读段过滤和质量修剪功能：

- **质量过滤**: 最小平均质量阈值
- **长度过滤**: 最小/最大读段长度筛选
- **N 碱基控制**: 最大 N 碱基比例过滤
- **质量修剪**: 从两端/5'/3' 端修剪低质量碱基
- **组合过滤**: 单次应用多个条件

```bash
# 质量和长度过滤
FastQTools filter -i input.fq.gz -o filtered.fq.gz \
  --min-quality 20 --min-length 50

# 质量修剪 + N 碱基过滤
FastQTools filter -i input.fq.gz -o trimmed.fq.gz \
  --trim-quality 20 --trim-end both --max-n-ratio 0.05
```

---

## 🚀 快速开始

### 一键构建

```bash
# 克隆并构建
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build

# 运行
./build/clang-release/FastQTools --help
```

### Docker（生产环境推荐）

```bash
# 从 Docker Hub 拉取
docker pull lessup/fastqtools:latest

# 直接运行
docker run lessup/fastqtools:latest FastQTools stat -i /data/input.fastq.gz -o /data/report.txt
```

---

## 📥 安装指南

<details>
<summary><b>🐧 Linux（推荐）</b></summary>

```bash
# 安装系统依赖
sudo apt-get update && sudo apt-get install -y \
  build-essential clang cmake git

# 构建 FastQTools
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build

# 验证安装
./build/clang-release/FastQTools --version
```

</details>

<details>
<summary><b>🍎 macOS</b></summary>

```bash
# 通过 Homebrew 安装依赖
brew install clang cmake conan

# 构建
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build --compiler clang
```

</details>

<details>
<summary><b>🐳 Docker</b></summary>

```bash
# 方式 1：拉取预构建镜像
docker pull lessup/fastqtools:latest

# 方式 2：从源码构建
cd fastq-tools/docker
docker-compose build

# 挂载卷运行
docker run -v /your/data:/data lessup/fastqtools:latest \
  FastQTools stat -i /data/input.fastq.gz -o /data/report.txt
```

</details>

<details>
<summary><b>📦 Conan 包管理器</b></summary>

```bash
# 在 conanfile.txt 中添加依赖
[requires]
fastqtools/3.1.0@lessup/stable

# 或从源码构建
conan create . --build=missing
```

</details>

---

## 📊 性能指标

在 AMD Ryzen 9 5900X 上的 **100K reads (150bp)** 基准测试结果：

| 操作 | 吞吐量 | 速度 | 内存 |
|------|--------|------|------|
| **FastQReader** | 1696 MB/s | 18.8 ms | ~50 MB |
| **FastQWriter** | 176万 reads/s | 57.0 ms | ~45 MB |
| **Filter Combined** | 167万 reads/s | 60.5 ms | ~60 MB |
| **Stat Full** | 302 MB/s | 104.6 ms | ~80 MB |

> **关键指标**: 并行流水线处理比顺序处理快 **5-10 倍**。

查看[详细基准测试报告](https://lessup.github.io/fastq-tools/performance/benchmark-reports/latest.md)了解更多场景。

---

## 💻 使用示例

### 基本操作

```bash
# 查看帮助
FastQTools --help

# 快速统计
FastQTools stat -i sample.fastq.gz -o stats.txt

# 过滤低质量读段
FastQTools filter -i sample.fastq.gz -o filtered.fastq.gz \
  --min-quality 20 --min-length 50
```

### 高级工作流

```bash
# 处理压缩文件（自动检测 gzip/bzip2）
FastQTools stat -i data/sample.fastq.gz -o report.txt

# 链式多重过滤
FastQTools filter -i input.fq.gz -o output.fq.gz \
  --min-quality 25 \
  --min-length 75 \
  --max-length 200 \
  --max-n-ratio 0.02 \
  --trim-quality 20 --trim-end both

# JSON 输出供程序调用
FastQTools stat -i data.fastq.gz -o stats.json --format json
```

### 集成到流水线

```bash
# 示例：NGS 质控流水线
#!/bin/bash

# 步骤 1：初始质控
FastQTools stat -i raw.fastq.gz -o raw_stats.txt

# 步骤 2：质量过滤
FastQTools filter -i raw.fastq.gz -o clean.fastq.gz \
  --min-quality 20 --min-length 50

# 步骤 3：过滤后质控
FastQTools stat -i clean.fastq.gz -o clean_stats.txt

# 步骤 4：生成对比报告
echo "原始读段: $(grep 'Total reads' raw_stats.txt)"
echo "干净读段: $(grep 'Total reads' clean_stats.txt)"
```

---

## 🛠️ 技术栈

| 类别 | 技术 | 版本 | 用途 |
|------|------|------|------|
| **语言** | C++ | 23 | 现代特性、概念、范围 |
| **并行** | Intel oneTBB | 2022.3.0 | 流水线并行 |
| **构建** | CMake + Ninja | 3.28+ | 快速增量构建 |
| **包管理** | Conan | 2.x | 依赖管理 |
| **压缩** | libdeflate + zlib-ng | 1.25 / 2.3.2 | 高性能 gzip |
| **日志** | spdlog | 1.17.0 | 异步日志 |
| **CLI** | cxxopts | 3.1.1 | 参数解析 |
| **测试** | GTest | 1.14+ | 单元/集成测试 |

---

## 📁 项目结构

```
fastq-tools/
├── 📄 README.md                   # 你在这里
├── 📚 docs/                       # 用户和开发者文档
│   ├── guide/                     # 用户指南
│   ├── dev/                       # 开发者指南
│   └── api/                       # API 参考
├── 🏗️ src/                        # 实现代码
│   ├── cli/commands/              # CLI 命令（stat, filter）
│   ├── io/                        # FASTQ I/O（reader, writer）
│   ├── processing/                # 流水线和修饰器
│   └── statistics/                # 统计计算器
├── 🔌 include/fqtools/            # 公共 API 头文件
├── 🧪 tests/                      # 测试套件
│   ├── unit/                      # 单元测试
│   ├── integration/               # 集成测试
│   └── e2e/                       # 端到端测试
├── 🛠️ scripts/core/               # 构建/测试/lint 脚本
├── 🐳 docker/                     # Docker 配置
└── 🔬 tools/                      # 开发工具（benchmark, fuzz）
```

---

## 📖 文档导航

| 资源 | 链接 |
|------|------|
| 🚀 **快速入门** | [指南](https://lessup.github.io/fastq-tools/guide/getting-started/) |
| 📖 **CLI 参考** | [命令](https://lessup.github.io/fastq-tools/guide/cli-reference/) |
| 🏗️ **架构设计** | [设计](https://lessup.github.io/fastq-tools/dev/architecture/) |
| 🔌 **API 参考** | [文档](https://lessup.github.io/fastq-tools/api/) |
| 📊 **性能报告** | [报告](https://lessup.github.io/fastq-tools/performance/benchmark-reports/latest.md) |
| 🤝 **贡献指南** | [指南](https://lessup.github.io/fastq-tools/contributing/) |
| 📝 **更新日志** | [历史](https://lessup.github.io/fastq-tools/changelog/) |

**完整在线文档**: <https://lessup.github.io/fastq-tools/>

---

## 🧪 质量保证

本项目通过以下方式维持高质量：

- ✅ **单元测试**: 所有模块基于 GTest 的单元测试
- ✅ **集成测试**: 端到端流水线验证
- ✅ **消毒剂**: CI 中运行 ASan、TSan、MSan、UBSan
- ✅ **模糊测试**: 持续对解析器进行模糊测试
- ✅ **代码覆盖率**: 强制执行覆盖率阈值
- ✅ **静态分析**: clang-tidy、cppcheck、IWYU

---

## 🤝 贡献

欢迎贡献！以下是参与方式：

1. 🐛 **报告错误**: [创建 Issue](https://github.com/LessUp/fastq-tools/issues/new)
2. 💡 **建议功能**: [GitHub 讨论](https://github.com/LessUp/fastq-tools/discussions)
3. 🔧 **提交代码**: [Fork & PR](https://github.com/LessUp/fastq-tools/fork)
4. 📚 **改进文档**: 修复拼写、添加示例、翻译
5. 🧪 **增加测试**: 提高测试覆盖率

查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详细指南。

### 贡献者快速开始

```bash
# 1. Fork 并克隆
git clone https://github.com/YOUR_USERNAME/fastq-tools.git
cd fastq-tools

# 2. 设置开发环境
./scripts/core/build --compiler clang --type Debug

# 3. 运行测试
./scripts/core/test

# 4. 修改并验证
./scripts/core/lint format
./scripts/core/test

# 5. 提交并推送
git commit -m "feat: 添加我的功能"
git push origin feat/my-feature
```

---

## 📜 许可证

本项目基于 [MIT 许可证](LICENSE) 授权。

```
MIT License
Copyright (c) 2025-2026 LessUp

特此免费授予任何获得本软件副本及相关文档文件（以下简称"软件"）的人...
```

---

## 🙏 致谢

- [Intel oneTBB](https://github.com/oneapi-src/oneTBB) - 并行流水线框架
- [libdeflate](https://github.com/ebiggers/libdeflate) - 高性能压缩库
- [spdlog](https://github.com/gabime/spdlog) - 异步日志库
- [Catch2](https://github.com/catchorg/Catch2) - 测试灵感来源

---

<p align="center">
  <sub>由 <a href="https://github.com/LessUp">LessUp</a> 团队用 ❤️ 打造</sub><br>
  <sub>用 ☕ 和 🍵 维护</sub>
</p>
