<h1 align="center">FastQTools</h1>

<p align="center">
  <b>A C++23 FASTQ quality-control tool with stat and filter commands.</b>
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

## Features

- `stat`: read counts, length distribution, base composition, GC content, Q20/Q30.
- `filter`: length/quality/N-ratio filtering; quality trimming (5'/3'/both); adapter trimming; polyG/polyX tail trimming — all in one streaming pass.

## Quick start

```bash
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build
./build/clang-release/FastQTools --help
```

Generate stats:

```bash
./build/clang-release/FastQTools stat \
  -i sample.fastq.gz \
  -o sample.stats.txt
```

Filter and trim:

```bash
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

Full options: [docs/cli-reference.md](./docs/cli-reference.md).

## Tech stack

| Layer | Technology |
|------|------|
| Language | C++23 (GCC 13+ / Clang 17+) |
| Build | CMake 3.28+ + Ninja |
| Package manager | Conan 2.x |
| Parallelism | Intel oneTBB |
| Compression | zlib-ng |
| Formatting | fmt |
| Testing | GoogleTest |

## Design highlights

- Zero-copy records: `FastqRecord` uses `std::string_view`s into a contiguous `FastqBatch` buffer; parsing is pointer arithmetic.
- Batch reuse: `FastqBatch::clear()` keeps capacity and works with an object pool to avoid per-batch allocation on the hot path.
- Three-stage pipeline: `serial_in_order (read) → parallel (filter/trim/stat) → serial_in_order (write + reduce)` keeps I/O ordered while compute scales across cores.
- Bounded memory: `maxLiveTokens` caps in-flight batches; `LowMemory` / `Default` / `HighThroughput` profiles derive the full working set.
- Small API surface: one `interfaces.h` per domain (io/processing/statistics); supports mock injection for tests.
- Atomic output: writes to a same-directory temp file and renames it on success; a failed run leaves the previous output intact.
- Error boundary: exception base class `FastQException` with `IOError` / `FormatError` / `ConfigurationError` subclasses; the CLI boundary catches and maps exit codes.

Full rationale: [docs/architecture.md](./docs/architecture.md).

## Performance

Environment: AMD Ryzen 7 5800H (WSL2, 8 cores / 16 threads), Clang 21 Release, 1M reads × 150 bp, seed=42, median of 5 repetitions.

| Scenario | Throughput |
|------|------|
| Reader | 202,903 reads/s (61.3 MiB/s) |
| plain writer (single API) | 117,444 reads/s (35.5 MiB/s) |
| gzip-6 writer (single API) | 7,194 reads/s (2.2 MiB/s) |
| filter baseline | 125,623 reads/s (38.0 MiB/s) |

Note: WSL2 inflates absolute `real_time`, so these numbers are for same-machine, same-command comparison only. Full data: [v4 baseline](./docs/performance/benchmark-reports/v4-baseline/2026-07-17/summary.md).

## Use cases

Good for:

- Embedding a lightweight FASTQ QC module into a C++ project.
- A small, auditable stat/filter component.
- A reference implementation of a C++23 streaming pipeline.

Not for: full adapter inference, visual reports, alignment, or variant calling. Use fastp, fastqc, or similar mature tools for that.

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

Fuzzer targets in `tools/fuzz/` exercise the FASTQ parser entry point.

## Documentation

| If you want to... | Start here |
| --- | --- |
| Build and run it | [docs/getting-started.md](./docs/getting-started.md) |
| Check command syntax | [docs/cli-reference.md](./docs/cli-reference.md) |
| Embed the library | [docs/api.md](./docs/api.md) |
| Understand the design | [docs/architecture.md](./docs/architecture.md) |
| Read benchmark data | [docs/benchmark.md](./docs/benchmark.md) |
| Contribute | [CONTRIBUTING.md](./CONTRIBUTING.md) |
| Track changes | [CHANGELOG.md](./CHANGELOG.md) |

## License

[MIT License](LICENSE).

## Author

**shijiashuai** — [GitHub](https://github.com/LessUp) · jiashuai.shi@qq.com
