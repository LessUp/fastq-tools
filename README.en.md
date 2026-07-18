<h1 align="center">FastQTools</h1>

<p align="center">
  <b>A small C++23 FASTQ quality-control tool</b><br>
  Does just stat and filter/trim, and tries to do them fast and reliably.
</p>

<p align="center">
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg" alt="CI Status">
  </a>
  <a href="https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml">
    <img src="https://img.shields.io/badge/build-GCC%20%7C%20Clang%20%7C%20ASan%20%7C%20TSan%20%7C%20UBSan-success?logo=githubactions" alt="CI Matrix">
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
  <a href="./docs/getting-started.md">Getting Started</a> ·
  <a href="./docs/architecture.md">Architecture</a> ·
  <a href="https://github.com/LessUp/fastq-tools/releases">Releases</a>
</p>

---

## What it does

The two most common QC operations on FASTQ files:

- **stat**: read counts, length distribution, base composition, GC content, Q20/Q30.
- **filter**: length, quality, and N-ratio filtering; quality trimming (5'/3'/both); adapter trimming; polyG/polyX tail trimming — all in one streaming pass.

## Quick start

```bash
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build
./build/clang-release/FastQTools --help
```

Generate a QC report:

```bash
./build/clang-release/FastQTools stat \
  -i sample.fastq.gz \
  -o sample.stats.txt
```

Filter and trim reads:

```bash
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

For setup, install notes, and the full option list, see [docs/getting-started.md](./docs/getting-started.md) and [docs/cli-reference.md](./docs/cli-reference.md).

## Design

FastQTools is not a Swiss-army tool. It is a focused, embeddable QC component.

- **Zero-copy record views**: `FastqRecord` holds a few `std::string_view`s into a contiguous batch buffer; parsing is pointer arithmetic, not string allocation.
- **Bounded streaming pipeline**: a three-stage oneTBB `parallel_pipeline` keeps I/O ordered while the CPU-bound stage scales across cores; memory is capped by `maxLiveTokens`.
- **Small API surface**: the public header is `<fqtools/fq.h>`; CLI and library share the same pipeline implementation.
- **Atomic output**: writes to a temp file in the same directory and renames it on success; a failed run leaves the previous output intact.

The full rationale is in [docs/architecture.md](./docs/architecture.md).

## When to use it

Good for:

- Embedding a lightweight FASTQ QC module into a C++ project.
- Needing a small, auditable stat/filter component.
- Reading the code as a reference for a C++23 streaming pipeline.

Not for:

- Full adapter inference, visual reports, alignment, or variant calling. Use fastp, fastqc, or similar mature tools for that.

## Build requirements

- C++23 compiler: **GCC 13+** or **Clang 17+**
- **CMake 3.28+**
- **Conan 2.x**
- Linux / macOS; on Windows use WSL or Docker

## Quality

CI is triggered manually on GitHub Actions and includes:

- Format: clang-format
- Static analysis: clang-tidy, cppcheck
- Builds: GCC Release, Clang Release
- Runtime checks: ASan, TSan, UBSan
- Tests: unit, integration, end-to-end
- Coverage: gcovr

Fuzzer targets in `tools/fuzz/` exercise the FASTQ parser entry point, the only place that consumes untrusted external input.

## Documentation

| If you want to... | Start here |
| --- | --- |
| Build and run it | [docs/getting-started.md](./docs/getting-started.md) |
| Check command syntax | [docs/cli-reference.md](./docs/cli-reference.md) |
| Embed the library | [docs/api.md](./docs/api.md) |
| Understand the design | [docs/architecture.md](./docs/architecture.md) |
| Read benchmark data | [docs/benchmark.md](./docs/benchmark.md) |
| Contribute | [CONTRIBUTING.md](./CONTRIBUTING.md) |
| Track project changes | [CHANGELOG.md](./CHANGELOG.md) |

## License

FastQTools is released under the [MIT License](LICENSE).

## Author

**shijiashuai** — [GitHub](https://github.com/LessUp) · jiashuai.shi@qq.com
