<h1 align="center">FastQTools</h1>

<p align="center">
  <b>Focused FASTQ quality-control toolkit</b><br>
  <i>Fast statistics, filtering, and trimming for sequencing data — with a CLI for day-to-day work and a C++ API for integration.</i>
</p>

<p align="center">
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg" alt="CI Status">
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
  <a href="./docs/architecture.md">Architecture</a> •
  <a href="https://github.com/LessUp/fastq-tools/releases">Releases</a>
</p>

---

FastQTools is a modern C++23 toolkit for routine FASTQ quality-control work. Its maintained surface is intentionally small: the `stat` command, the `filter` command, and a minimal embeddable C++ API centered on `fqtools/fq.h`, the documented common/config/error/io/processing headers it aggregates, and the supported statistics workflow `StatisticOptions + createStatisticCalculator(...)->run()`.

## What it is good at

- **FASTQ statistics (`stat`)** — inspect read counts, length distribution, base composition, GC content, and Q20/Q30-style quality metrics.
- **Filtering and trimming (`filter`)** — apply length, quality, and N-ratio thresholds, then trim low-quality bases in the same pass.
- **Embeddable C++ API (`fqtools/fq.h`)** — integrate through the umbrella header, the documented common/config/error/io/processing headers it aggregates, and the supported statistics workflow `StatisticOptions + createStatisticCalculator(...)->run()`.
- **Performance-conscious implementation** — zero-copy record views, Intel oneTBB pipelines, and compressed-file workflows where supported.
- **Production hygiene** — CI, sanitizers, fuzzing, and in-repo technical docs.

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

For environment setup, installation notes, and deployment options, see [docs/getting-started.md](./docs/getting-started.md).

## Representative performance

Benchmarks below are a maintained point-in-time snapshot for **100K reads (150 bp)** on an **AMD Ryzen 9 5900X**. They are useful for sizing expectations, not as blanket guarantees for every dataset or storage setup.

| Workload | Representative result |
| --- | --- |
| FASTQ read path | 1696 MB/s |
| FASTQ write path | 1.76M reads/s |
| Combined filtering pass | 1.67M reads/s |
| Full statistics pass | 302 MB/s |

See [docs/benchmark.md](./docs/benchmark.md) for methodology and context.

## Documentation map

| If you want to… | Start here |
| --- | --- |
| Build FastQTools and run your first command | [docs/getting-started.md](./docs/getting-started.md) |
| Check command syntax and options | [docs/cli-reference.md](./docs/cli-reference.md) |
| Integrate the library into C++ code | [docs/api.md](./docs/api.md) |
| Understand the architecture and design decisions | [docs/architecture.md](./docs/architecture.md) |
| Understand benchmark numbers | [docs/benchmark.md](./docs/benchmark.md) |
| Contribute docs or code | [CONTRIBUTING.md](./CONTRIBUTING.md) |
| Follow project-level changes | [CHANGELOG.md](./CHANGELOG.md) |

## Build requirements

- C++23-compatible compiler (**GCC 11+** or **Clang 12+** minimum)
- **CMake 3.28+**
- **Conan 2.x**
- Supported build environments today include Linux and macOS; on Windows, use Docker or WSL.

## Contributing

FastQTools welcomes focused improvements: bug reports, docs fixes, tests, benchmark work, and narrow feature changes.

- Read the [contributing guide](./CONTRIBUTING.md)
- Open issues via [GitHub Issues](https://github.com/LessUp/fastq-tools/issues)
- Discuss ideas in [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions)

## License

FastQTools is released under the [MIT License](LICENSE).

## Author

**shijiashuai** — [GitHub](https://github.com/LessUp) · jiashuai.shi@qq.com

A C++23 engineering showcase project: zero-copy FASTQ views, TBB pipeline parallelism, interface minimalism, and dependency trimming. See [docs/architecture.md](./docs/architecture.md) for the technical narrative.
