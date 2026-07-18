<h1 align="center">FastQTools</h1>

<p align="center">
  <b>A C++23 parallel pipeline framework. FASTQ QC is its primary application.</b>
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
  <img src="https://img.shields.io/badge/CMake-3.28%2B-064F8C?logo=cmake" alt="CMake 3.28+">
</p>

<p align="center">
  <a href="README.md">中文</a> ·
  <a href="README.en.md">English</a> ·
  <a href="./docs/getting-started.md">Getting Started</a> ·
  <a href="./docs/architecture.md">Architecture</a> ·
  <a href="https://github.com/LessUp/fastq-tools/releases">Releases</a>
</p>

---

## Why FastQTools

The FASTQ QC space is crowded — fastp does it all, fastqc visualizes everything. FastQTools takes a different path: it's not just another QC tool. It's a **parallel pipeline framework** where FASTQ processing happens to be the first application.

- **A reusable pipeline framework**: `ExecutionRuntime` + `ExecutionBackend` (Sequential / oneTBB). Adapter pattern for plugging in arbitrary batch computation — not tied to FASTQ.
- **Extensible operator system**: `Predicate` and `Mutator` interfaces with strategy-pattern composition. Dependency injection for testing.
- **FASTQ QC as primary application**: `stat` (statistics) and `filter` (filtering/trimming). Deterministic, explicit-parameter, fully reproducible. No heuristics, no auto-detection magic.
- **C++23 engineering reference**: real-world `tbb::parallel_pipeline` + zero-copy `string_view` + bounded memory.

## Architecture at a glance

```
  Reader                 Worker Pool              Writer
  ──────                ───────────              ──────
  serial_in_order  →    parallel        →        serial_in_order
  (gzip/zlib-ng)        (predicates + mutators)  (write + reduce)
```

### Framework layers

| Layer | Responsibility | Extension point |
|---|---|---|
| ExecutionRuntime | Accepts any Adapter, selects backend, manages lifecycle | Implement Adapter contract |
| ExecutionBackend | Concrete scheduling framework (Sequential / oneTBB) | Add new Backend implementation |
| ExecutionOperation | Type-erased batch processing contract | Data-type-agnostic |
| Pipeline (FASTQ) | FASTQ-specific pipeline, registers Predicates / Mutators | Implement and inject interfaces |

### Key design points

| Aspect | Detail |
|--------|--------|
| Zero-copy | `FastqRecord` fields are `std::string_view` into a `FastqBatch` buffer; `clear()` retains capacity for reuse without allocation. |
| Three-stage pipeline | `tbb::parallel_pipeline`: read → parallel process → write. Read and write stages are serial and order-preserving; the middle stage is lock-free parallel. |
| Bounded memory | `maxLiveTokens` caps in-flight batches, derived from a working-set profile. Insufficient budget raises `ConfigurationError` — no silent OOM. |
| Error boundary | `FastQException` with `IOError` / `FormatError` / `ConfigurationError` subclasses. CLI boundary catches and maps to exit codes. |

See [architecture docs](./docs/architecture.md).

## Core application: FASTQ QC

### `stat`

Read count, length distribution, base composition, GC content, Q20/Q30.

### `filter`

Length filtering, quality filtering, N-ratio filtering; 5'/3'/both-end quality trimming; adapter trimming; polyG/polyX tail trimming. All in one streaming pass.

## Extensibility

FastQTools is not a black box. You can:

- **Add custom filter/transform logic**: implement `ReadPredicateInterface` or `ReadMutatorInterface`, register via `Pipeline::addReadPredicate()` / `addReadMutator()`.
- **Custom I/O**: implement `IReader` / `IWriter`, inject via `Pipeline::setReader()` / `setWriter()`.
- **Custom backend**: implement `ExecutionBackend` to plug in your own scheduling framework.
- **Other data formats**: implement the Adapter contract (`makeResult` / `processBatch` / `afterCommit` / `merge`), reuse `ExecutionRuntime`.

```cpp
// Custom quality filter example
class MyQualityFilter final : public fq::processing::ReadPredicateInterface {
public:
    [[nodiscard]] auto evaluate(const fq::io::FastqRecord& read) const -> bool override {
        // Your filter logic
        return true;
    }
};

fq::processing::Pipeline pipeline;
pipeline.addReadPredicate(std::make_unique<MyQualityFilter>());
auto stats = pipeline.run();
```

## Quick start

**Prerequisites**: GCC 13+ / Clang 17+, CMake 3.28+, Conan 2.x. Linux / macOS; use WSL or Docker on Windows.

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

Full options: [CLI reference](./docs/cli-reference.md).

## Tech stack

| Layer | Technology |
|-------|------------|
| Language | C++23 (GCC 13+ / Clang 17+) |
| Build | CMake 3.28+ · Ninja |
| Package manager | Conan 2.x |
| Parallelism | Intel oneTBB |
| Compression | zlib-ng |
| Formatting | fmt |
| Testing | GoogleTest |

## Performance

Environment: AMD Ryzen 7 5800H (WSL2, 8 cores / 16 threads), Clang 21 Release, 1M reads × 150 bp, seed=42, median of 5 repetitions.

| Scenario | Throughput |
|----------|------------|
| Reader | 202,903 reads/s (61.3 MiB/s) |
| plain writer (single API) | 117,444 reads/s (35.5 MiB/s) |
| gzip-6 writer (single API) | 7,194 reads/s (2.2 MiB/s) |
| filter baseline | 125,623 reads/s (38.0 MiB/s) |

Same-machine, same-command comparison only. Full data: [v4 baseline](./docs/performance/benchmark-reports/v4-baseline/2026-07-17/summary.md).

## Quality assurance

CI (GitHub Actions, manual trigger) covers:

- Static analysis: clang-format, clang-tidy, cppcheck
- Multi-compiler: GCC + Clang, Release mode
- Sanitizers: ASan, TSan, UBSan
- Tests: unit, integration, E2E, gcovr coverage
- Fuzzer: `tools/fuzz/` runs coverage-guided fuzzing against the FASTQ parser

## Comparison

| | FastQTools | fastp | fastqc |
|---|---|---|---|
| Positioning | Parallel pipeline framework + QC app | Full-featured FASTQ QC | Quality report |
| Statistics | Q20/Q30, GC, length, base composition | Richer QC metrics | Visual HTML report |
| Filter/trim | Single pass, fully configurable | Auto adapter detection | Not supported |
| Adapter inference | No (explicit sequence required) | Built-in auto-detection | Not supported |
| Visualization | None | HTML + JSON report | Interactive HTML |
| Parallel model | tbb::parallel_pipeline | Thread pool | Single-threaded |
| Embeddable | Library API + interface injection | CLI tool | CLI tool |
| Language | C++23 | C++11 | Java |

Need auto adapter detection or visual reports? Use fastp or fastqc. Need deterministic, reproducible, high-performance stat + filter — or want to build your own batch processing app on a parallel pipeline framework? Use FastQTools.

## Documentation

| Goal | Start here |
| --- | --- |
| Command syntax | [CLI reference](./docs/cli-reference.md) |
| C++ API | [API overview](./docs/api.md) |
| Architecture & design | [Architecture docs](./docs/architecture.md) |
| Benchmark data | [Benchmark overview](./docs/benchmark.md) |
| Contribute | [Contributing guide](./CONTRIBUTING.md) |
| Changelog | [CHANGELOG.md](./CHANGELOG.md) |

## License

[MIT License](LICENSE).

## Author

**shijiashuai** — [GitHub](https://github.com/LessUp)
