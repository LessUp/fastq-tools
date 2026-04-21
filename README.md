<h1 align="center">FastQTools</h1>

<p align="center">
  <b>High-Performance FASTQ File Processing Toolkit</b><br>
  <i>Modern C++23 Bioinformatics QC Toolkit with Zero-Copy I/O and Parallel Processing</i>
</p>

<p align="center">
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg" alt="CI Status">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml/badge.svg" alt="Docs Status">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/releases">
    <img src="https://img.shields.io/github/v/release/LessUp/fastq-tools?label=Release&logo=github" alt="GitHub Release">
  </a>
  <a href="https://opensource.org/licenses/MIT">
    <img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License: MIT">
  </a>
  <img src="https://img.shields.io/badge/C%2B%2B-23-blue.svg" alt="C++23">
  <a href="https://github.com/LessUp/fastq-tools/discussions">
    <img src="https://img.shields.io/badge/GitHub-Discussions-blue?logo=github" alt="GitHub Discussions">
  </a>
</p>

<p align="center">
  <a href="README.md">English</a> •
  <a href="README.zh-CN.md">简体中文</a> •
  <a href="https://lessup.github.io/fastq-tools/">Documentation</a> •
  <a href="https://github.com/LessUp/fastq-tools/releases">Releases</a>
</p>

---

## 🌟 Overview

FastQTools is a **high-performance toolkit for processing FASTQ files** (sequencing data) built with **modern C++23**. It provides:

- ⚡ **Extreme Performance**: Up to **1.7M reads/second** with parallel TBB pipelines
- 🚀 **Zero-Copy I/O**: `std::string_view`-based record handling for minimal memory overhead
- 🔧 **Production Ready**: Comprehensive sanitizers, fuzzing, and CI/CD validation
- 📦 **Developer Friendly**: CMake + Conan build system, clean API, and Docker support

Perfect for bioinformatics pipelines, NGS data processing, and genomic QC workflows.

---

## ✨ Features

### 📊 Statistical Analysis (`stat`)

Comprehensive FASTQ file statistics in a single command:

- **Read Metrics**: Total count, length distribution, average/median/min/max
- **Base Composition**: A/T/C/G/N ratios per position
- **Quality Analysis**: Q20/Q30 percentages, quality score distribution
- **GC Content**: GC percentage and distribution curve
- **Output Formats**: Human-readable text, JSON for programmatic access

```bash
FastQTools stat -i input.fastq.gz -o report.txt
FastQTools stat -i input.fastq.gz -o report.json --format json
```

### 🔍 Filtering & Trimming (`filter`)

Flexible read filtering and quality trimming:

- **Quality Filter**: Minimum average quality threshold
- **Length Filter**: Min/max read length selection
- **N-Base Control**: Maximum N-base ratio filtering
- **Quality Trimming**: Trim low-quality bases from both/5'/3' ends
- **Combined Filters**: Apply multiple criteria in single pass

```bash
# Quality and length filtering
FastQTools filter -i input.fq.gz -o filtered.fq.gz \
  --min-quality 20 --min-length 50

# Quality trimming + N-base filtering
FastQTools filter -i input.fq.gz -o trimmed.fq.gz \
  --trim-quality 20 --trim-end both --max-n-ratio 0.05
```

---

## 🚀 Quick Start

### One-Command Build

```bash
# Clone and build
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build

# Run
./build/clang-release/FastQTools --help
```

### Docker (Recommended for Production)

```bash
# Pull from Docker Hub
docker pull lessup/fastqtools:latest

# Run directly
docker run lessup/fastqtools:latest FastQTools stat -i /data/input.fastq.gz -o /data/report.txt
```

---

## 📥 Installation

<details>
<summary><b>🐧 Linux (Recommended)</b></summary>

```bash
# Install system dependencies
sudo apt-get update && sudo apt-get install -y \
  build-essential clang cmake git

# Build FastQTools
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build

# Verify installation
./build/clang-release/FastQTools --version
```

</details>

<details>
<summary><b>🍎 macOS</b></summary>

```bash
# Install dependencies via Homebrew
brew install clang cmake conan

# Build
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build --compiler clang
```

</details>

<details>
<summary><b>🐳 Docker</b></summary>

```bash
# Option 1: Pull pre-built image
docker pull lessup/fastqtools:latest

# Option 2: Build from source
cd fastq-tools/docker
docker-compose build

# Run with volume mount
docker run -v /your/data:/data lessup/fastqtools:latest \
  FastQTools stat -i /data/input.fastq.gz -o /data/report.txt
```

</details>

<details>
<summary><b>📦 Conan Package Manager</b></summary>

```bash
# Add as dependency in your conanfile.txt
[requires]
fastqtools/3.1.0@lessup/stable

# Or build from source
conan create . --build=missing
```

</details>

---

## 📊 Performance

Benchmark results with **100K reads (150bp)** on AMD Ryzen 9 5900X:

| Operation | Throughput | Speed | Memory |
|-----------|-----------|-------|--------|
| **FastQReader** | 1696 MB/s | 18.8 ms | ~50 MB |
| **FastQWriter** | 1.76M reads/s | 57.0 ms | ~45 MB |
| **Filter Combined** | 1.67M reads/s | 60.5 ms | ~60 MB |
| **Stat Full** | 302 MB/s | 104.6 ms | ~80 MB |

> **Key Insight**: Parallel pipeline processing achieves **5-10x speedup** over sequential processing.

See [detailed benchmark reports](https://lessup.github.io/fastq-tools/performance/benchmark-reports/latest.md) for more scenarios.

---

## 💻 Usage Examples

### Basic Operations

```bash
# View help
FastQTools --help

# Quick statistics
FastQTools stat -i sample.fastq.gz -o stats.txt

# Filter low-quality reads
FastQTools filter -i sample.fastq.gz -o filtered.fastq.gz \
  --min-quality 20 --min-length 50
```

### Advanced Workflows

```bash
# Process compressed files (auto-detect gzip/bzip2)
FastQTools stat -i data/sample.fastq.gz -o report.txt

# Chain multiple filters
FastQTools filter -i input.fq.gz -o output.fq.gz \
  --min-quality 25 \
  --min-length 75 \
  --max-length 200 \
  --max-n-ratio 0.02 \
  --trim-quality 20 --trim-end both

# JSON output for programmatic processing
FastQTools stat -i data.fastq.gz -o stats.json --format json
```

### Integration in Pipelines

```bash
# Example: NGS QC pipeline
#!/bin/bash

# Step 1: Initial QC
FastQTools stat -i raw.fastq.gz -o raw_stats.txt

# Step 2: Quality filtering
FastQTools filter -i raw.fastq.gz -o clean.fastq.gz \
  --min-quality 20 --min-length 50

# Step 3: Post-filtering QC
FastQTools stat -i clean.fastq.gz -o clean_stats.txt

# Step 4: Generate comparison report
echo "Raw reads: $(grep 'Total reads' raw_stats.txt)"
echo "Clean reads: $(grep 'Total reads' clean_stats.txt)"
```

---

## 🛠️ Tech Stack

| Category | Technology | Version | Purpose |
|----------|-----------|---------|---------|
| **Language** | C++ | 23 | Modern features, concepts, ranges |
| **Parallel** | Intel oneTBB | 2022.3.0 | Pipeline parallelism |
| **Build** | CMake + Ninja | 3.28+ | Fast incremental builds |
| **Package** | Conan | 2.x | Dependency management |
| **Compression** | libdeflate + zlib-ng | 1.25 / 2.3.2 | High-performance gzip |
| **Logging** | spdlog | 1.17.0 | Async logging |
| **CLI** | cxxopts | 3.1.1 | Argument parsing |
| **Testing** | GTest | 1.14+ | Unit/integration tests |

---

## 📁 Project Structure

```
fastq-tools/
├── 📄 README.md                   # You are here
├── 📚 docs/                       # User & developer docs
│   ├── guide/                     # User guides
│   ├── dev/                       # Developer guides
│   └── api/                       # API reference
├── 🏗️ src/                        # Implementation
│   ├── cli/commands/              # CLI commands (stat, filter)
│   ├── io/                        # FASTQ I/O (reader, writer)
│   ├── processing/                # Pipeline & mutators
│   └── statistics/                # Statistical calculators
├── 🔌 include/fqtools/            # Public API headers
├── 🧪 tests/                      # Test suite
│   ├── unit/                      # Unit tests
│   ├── integration/               # Integration tests
│   └── e2e/                       # End-to-end tests
├── 🛠️ scripts/core/               # Build/test/lint scripts
├── 🐳 docker/                     # Docker configs
└── 🔬 tools/                      # Dev tools (benchmark, fuzz)
```

---

## 📖 Documentation

| Resource | Link |
|----------|------|
| 🚀 **Getting Started** | [Guide](https://lessup.github.io/fastq-tools/guide/getting-started/) |
| 📖 **CLI Reference** | [Commands](https://lessup.github.io/fastq-tools/guide/cli-reference/) |
| 🏗️ **Architecture** | [Design](https://lessup.github.io/fastq-tools/dev/architecture/) |
| 🔌 **API Reference** | [API Docs](https://lessup.github.io/fastq-tools/api/) |
| 📊 **Benchmarks** | [Reports](https://lessup.github.io/fastq-tools/performance/benchmark-reports/latest.md) |
| 🤝 **Contributing** | [Guide](https://lessup.github.io/fastq-tools/contributing/) |
| 📝 **Changelog** | [History](https://lessup.github.io/fastq-tools/changelog/) |

**Full Documentation**: <https://lessup.github.io/fastq-tools/>

---

## 🧪 Quality Assurance

This project maintains high quality through:

- ✅ **Unit Tests**: GTest-based unit tests for all modules
- ✅ **Integration Tests**: End-to-end pipeline validation
- ✅ **Sanitizers**: ASan, TSan, MSan, UBSan in CI
- ✅ **Fuzzing**: Continuous fuzz testing for parsers
- ✅ **Code Coverage**: Coverage thresholds enforced
- ✅ **Static Analysis**: clang-tidy, cppcheck, IWYU

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

1. 🐛 **Report bugs**: [Open an issue](https://github.com/LessUp/fastq-tools/issues/new)
2. 💡 **Suggest features**: [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions)
3. 🔧 **Submit code**: [Fork & PR](https://github.com/LessUp/fastq-tools/fork)
4. 📚 **Improve docs**: Fix typos, add examples, translate
5. 🧪 **Add tests**: Increase test coverage

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

### Quick Start for Contributors

```bash
# 1. Fork and clone
git clone https://github.com/YOUR_USERNAME/fastq-tools.git
cd fastq-tools

# 2. Set up development environment
./scripts/core/build --compiler clang --type Debug

# 3. Run tests
./scripts/core/test

# 4. Make changes and verify
./scripts/core/lint format
./scripts/core/test

# 5. Commit and push
git commit -m "feat: add my feature"
git push origin feat/my-feature
```

---

## 📜 License

This project is licensed under the [MIT License](LICENSE).

```
MIT License
Copyright (c) 2025-2026 LessUp

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...
```

---

## 🙏 Acknowledgments

- [Intel oneTBB](https://github.com/oneapi-src/oneTBB) - Parallel pipeline framework
- [libdeflate](https://github.com/ebiggers/libdeflate) - High-performance compression
- [spdlog](https://github.com/gabime/spdlog) - Async logging library
- [Catch2](https://github.com/catchorg/Catch2) - Testing inspiration

---

<p align="center">
  <sub>Built with ❤️ by the <a href="https://github.com/LessUp">LessUp</a> team</sub><br>
  <sub>Maintained with ☕ and 🍵</sub>
</p>
