<h1 align="center">FastQTools</h1>

<p align="center">
  <b>Focused FASTQ quality-control toolkit</b><br>
  <i>Fast statistics, filtering, and trimming for sequencing data — with a CLI for day-to-day work and a C++ API for integration.</i>
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

FastQTools is a modern C++23 toolkit for routine FASTQ quality-control work. It is built for researchers and pipeline engineers who want a focused tool for inspecting, filtering, and trimming reads — not a heavyweight workflow platform.

## What it is good at

- **FASTQ statistics (`stat`)** — inspect read counts, length distribution, base composition, GC content, and Q20/Q30-style quality metrics.
- **Filtering and trimming (`filter`)** — apply length, quality, and N-ratio thresholds, then trim low-quality bases in the same pass.
- **Pipeline integration** — use the CLI directly or embed the public C++ API in larger bioinformatics tools.
- **Performance-conscious implementation** — zero-copy record views, Intel oneTBB pipelines, and compressed-file workflows where supported.
- **Production hygiene** — CI, sanitizers, fuzzing, and a maintained docs site.

## Quick start

```bash
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build
./build/clang-release/FastQTools --help

# Generate a QC report
./build/clang-release/FastQTools stat \
  -i sample.fastq.gz \
  -o sample.stats.txt

# Filter and trim reads
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

For environment setup, installation notes, and deployment options, start with the [Getting Started guide](https://lessup.github.io/fastq-tools/guide/getting-started.en/).

## Representative performance

Benchmarks below are a maintained point-in-time snapshot for **100K reads (150 bp)** on an **AMD Ryzen 9 5900X**. They are useful for sizing expectations, not as blanket guarantees for every dataset or storage setup.

| Workload | Representative result |
| --- | --- |
| FASTQ read path | 1696 MB/s |
| FASTQ write path | 1.76M reads/s |
| Combined filtering pass | 1.67M reads/s |
| Full statistics pass | 302 MB/s |

See the [benchmark overview](https://lessup.github.io/fastq-tools/performance/benchmark-report/) for methodology and context.

## Documentation map

| If you want to… | Start here |
| --- | --- |
| Build FastQTools and run your first command | [Getting Started](https://lessup.github.io/fastq-tools/guide/getting-started.en/) |
| Check command syntax and options | [CLI Reference](https://lessup.github.io/fastq-tools/guide/cli-reference.en/) |
| Integrate the library into C++ code | [API Overview](https://lessup.github.io/fastq-tools/api/overview.en/) |
| Understand benchmark numbers | [Benchmark Overview](https://lessup.github.io/fastq-tools/performance/benchmark-report/) |
| Contribute docs or code | [Contributing](https://lessup.github.io/fastq-tools/contributing.en/) |
| Follow project-level changes | [Changelog](https://lessup.github.io/fastq-tools/changelog.en/) |

## Build requirements

- C++23-compatible compiler (**GCC 11+** or **Clang 12+** minimum)
- **CMake 3.28+**
- **Conan 2.x**
- Linux is the primary target; macOS is supported for local development, and Windows users should prefer Docker or WSL.

## Contributing

FastQTools welcomes focused improvements: bug reports, docs fixes, tests, benchmark work, and narrow feature changes.

- Read the [contributing guide](https://lessup.github.io/fastq-tools/contributing.en/)
- Open issues via [GitHub Issues](https://github.com/LessUp/fastq-tools/issues)
- Discuss ideas in [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions)

## License

FastQTools is released under the [MIT License](LICENSE).
