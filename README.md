<h1 align="center">FastQTools</h1>

<p align="center">
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
  <a href="README.md">English</a> | <a href="README.zh-CN.md">简体中文</a>
</p>

<p align="center">
  <a href="https://lessup.github.io/fastq-tools/" style="font-size: 1.1em;">
    📚 <b>Online Documentation / 在线文档</b>
  </a>
</p>

<p align="center">
  <a href="specs/README.md">📋 Specifications / 规范文档</a>
</p>

---

## ✨ Core Features

<table>
<tr>
<td width="50%">

### 📊 `stat` — Statistical Analysis

Comprehensive FASTQ file statistical analysis:
- Total reads, length distribution
- Base composition (A/T/C/G/N ratios)
- GC content, Q20/Q30 percentages

```bash
FastQTools stat -i input.fastq.gz -o report.txt
```

</td>
<td width="50%">

### 🔍 `filter` — Filtering & Trimming

Read filtering and quality trimming:
- Quality threshold filtering
- Length range selection
- N-base ratio control
- Quality trimming (both/5'/3' ends)

```bash
FastQTools filter -i input.fq.gz -o output.fq.gz \
  --min-quality 20 --min-length 50
```

</td>
</tr>
</table>

---

## 🚀 Quick Start

```bash
# 1. Clone repository
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools

# 2. Install dependencies
./scripts/core/install-deps

# 3. One-command build
./scripts/core/build

# 4. Show help
./build/clang-release/FastQTools --help

# 5. Run tests
./scripts/core/test
```

---

## 📊 Performance

Benchmark with 100K reads (150bp):

| Operation | Throughput | Performance Level |
|-----------|------------|-------------------|
| FastQReader | 1696 MB/s | 🟢 Extreme |
| FastQWriter | 1.76 M reads/s | 🟢 Extreme |
| Filter Combined | 1.67 M reads/s | 🟢 Extreme |
| Stat Full | 302 MB/s | 🟡 High |

> Test Env: AMD Ryzen 9 5900X, 32GB DDR4, NVMe SSD

---

## 🛠️ Tech Stack

| Category | Technology | Version |
|----------|------------|---------|
| **Language** | C++ | 23 |
| **Parallel** | Intel oneTBB | 2022.3.0 |
| **Build** | CMake | 3.28+ |
| **Package** | Conan | 2.x |
| **Compression** | libdeflate/zlib-ng | 1.25/2.3.2 |
| **Logging** | spdlog | 1.17.0 |
| **CLI** | cxxopts | 3.1.1 |

---

## 📁 Project Structure

```
fastq-tools/
├── include/fqtools/     # Public API headers
├── src/                 # Source implementation
│   ├── cli/             # CLI entry
│   ├── io/              # FASTQ I/O
│   ├── processing/      # Processing pipeline
│   └── statistics/      # Statistics
├── tests/               # Tests (unit / integration / e2e)
├── docs/                # Documentation (Bilingual)
├── changelog/           # Change records
├── scripts/             # Build scripts
├── docker/              # Docker configs
└── tools/               # Dev tools (benchmark / fuzz)
```

---

## 📖 Documentation

| Document | Link |
|----------|------|
| Getting Started | [docs/guide/getting-started.en.md](docs/guide/getting-started.en.md) |
| CLI Reference | [docs/guide/cli-reference.en.md](docs/guide/cli-reference.en.md) |
| Architecture | [docs/dev/architecture.en.md](docs/dev/architecture.en.md) |
| API Reference | [docs/api/overview.en.md](docs/api/overview.en.md) |

**Full Docs**: [https://lessup.github.io/fastq-tools/](https://lessup.github.io/fastq-tools/)

---

## 🤝 Contributing

Welcome to submit Issues and Pull Requests!

See [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines.

---

## 📜 License

[MIT License](LICENSE) © 2025-2026 LessUp

---

<p align="center">
  <sub>Built with ❤️ by the LessUp team</sub>
</p>
