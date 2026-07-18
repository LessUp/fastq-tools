<h1 align="center">FastQTools</h1>

<p align="center">
  <b>A focused, modern C++23 FASTQ quality-control toolkit</b><br>
  <i>Zero-copy record views, a bounded streaming pipeline, and a small embeddable API surface — built to do a few QC things well.</i>
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
  <a href="README.md">English</a> •
  <a href="README.zh-CN.md">简体中文</a> •
  <a href="./docs/architecture.md">Architecture</a> •
  <a href="https://github.com/LessUp/fastq-tools/releases">Releases</a>
</p>

---

## Table of contents

- [Why FastQTools](#why-fastqtools)
- [Where it fits](#where-it-fits)
- [Highlights](#highlights)
- [Quick start](#quick-start)
- [Embeddable C++ API](#embeddable-c-api)
- [Architecture at a glance](#architecture-at-a-glance)
- [Representative performance](#representative-performance)
- [Quality bar](#quality-bar)
- [Documentation map](#documentation-map)
- [Build requirements](#build-requirements)
- [Contributing](#contributing)
- [License](#license)
- [Author](#author)

---

## Why FastQTools

Most FASTQ QC tools optimize for breadth: many features, many report formats, many edge cases. FastQTools goes the other way. It is a **C++23 engineering showcase** that deliberately keeps the maintenance surface small — `stat`, `filter`, and one umbrella header — and puts the engineering budget into the parts that are usually invisible:

- A zero-copy record model where parsing collapses to pointer arithmetic.
- A private execution-backend seam with an ordered oneTBB pipeline by default; v4 keeps only Sequential and oneTBB.
- A batch + object-pool memory discipline that avoids per-record allocation on the hot path.
- A manually triggered CI matrix that runs GCC, Clang, ASan, TSan, and UBSan when started from the GitHub Actions UI, plus fuzzer targets at the parser entry (CI integration pending).

The thesis is simple: **doing a few things well is harder than doing many things adequately**. This repository is that argument in code.

## Where it fits

FastQTools targets the two highest-frequency operations in everyday sequencing QC — summary statistics and read filtering/trimming — and a minimal embeddable C++ API for integrating them into larger pipelines. It deliberately does **not** do alignment, variant calling, assembly, or visualization.

| Tool | Scope | Language | Strength | FastQTools vs. it |
| --- | --- | --- | --- | --- |
| **fastp** | All-in-one QC + adapter/trimming + report | C++11 | Mature, feature-rich, de facto standard | Smaller surface; modern C++23 core; embeddable API; stricter CI/sanitizers |
| **fastqc** | QC report only (Java) | Java | Rich visual report, broad adoption | CLI-native; composable; no JVM; C++ embeddable |
| **seqkit** | Swiss-army SEQ toolkit (Go) | Go | Huge operator surface, scripting-friendly | Focused on QC hot path; native C++ throughput; smaller binary |
| **FastQTools** | `stat` + `filter` + embeddable API | C++23 | Zero-copy + TBB streaming + sanitizer-hardened | — |

FastQTools is not a drop-in fastp replacement. It is a focused, modern-core alternative when you want a small, auditable, embeddable QC component — or a reference for how a C++23 streaming pipeline is built.

## Highlights

- **`stat` — FASTQ statistics.** Read counts, length distribution, base composition, GC content, Q20/Q30-style quality metrics, optional signature sidecar report (head k-mer, duplicate estimation).
- **`filter` — filtering and trimming in one pass.** Length, average quality, N-ratio thresholds; quality trimming (5'/3'/both); adapter trimming; polyG / polyX tail trimming — all in a single streaming scan.
- **Embeddable C++ API.** One umbrella header `<fqtools/fq.h>`; CLI and library share the same pipeline, so behavior is identical.
- **Zero-copy record views.** `FastqRecord` is five `std::string_view`s pointing into a contiguous batch buffer; parsing is pointer arithmetic, not allocation.
- **Bounded streaming pipeline.** The oneTBB path uses `serial_in_order → parallel → serial_in_order`, keeping I/O ordered and reduction deterministic while the CPU-bound middle stage scales across cores. A sequential backend remains available as the contract baseline.
- **Sanitizer-hardened CI.** A manually triggered run executes GCC Release, Clang Release, Clang ASan/TSan/UBSan, clang-tidy, and cppcheck; fuzzer targets live in `tools/fuzz/` (CI integration pending).

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

Filter and trim reads in one pass:

```bash
./build/clang-release/FastQTools filter \
  -i sample.fastq.gz \
  -o sample.filtered.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

For environment setup, install notes, and deployment options, see [docs/getting-started.md](./docs/getting-started.md). For full option reference, see [docs/cli-reference.md](./docs/cli-reference.md).

## Embeddable C++ API

The CLI and the library share the same pipeline. Embedding gives you the same behavior without a subprocess.

```cpp
#include <fqtools/fq.h>

// Statistics
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

// Filter + trim, composable via predicates and mutators
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

`Pipeline` is move-only and hides its implementation behind PIMPL. `setReader` / `setWriter` accept `unique_ptr<IReader>` / `IWriter`, so tests can inject mocks. See [docs/api.md](./docs/api.md) for the full header map.

### v4 contract

FastQTools 4.0.0 is intentionally a breaking release. The public factory functions are replaced by move-only `fq::processing::Pipeline` and `fq::statistics::Calculator`; statistics use `fq::statistics`; config, legacy logging, public object pools, and the Taskflow backend are no longer public or built. The installed CMake consumer target is `FastQTools::FastQTools`.

`IWriter::write()` means that a batch was accepted by the writer. Call `IWriter::finish()` exactly once to flush buffers, close compression, and publish a default `FastqWriter` output. The default writer writes a same-directory temporary file and atomically renames it only after a successful finish; failed output leaves the previous target intact and removes the temporary file.

`LowMemory`, `Default`, and `HighThroughput` profiles account for batch storage, record views, reader remainder, and writer/zlib buffers when deriving `maxLiveTokens`. A memory limit smaller than one runtime working set raises `ConfigurationError` instead of silently exceeding the limit.

## Architecture at a glance

```
                 ┌─────────── src/cli ────────────┐
                 │  main → CommandRegistry         │
                 │   ├─ StatCommand                │
                 │   └─ FilterCommand              │
                 └────────┬────────────────────────┘
                          │ assemble options + inject deps
          ┌───────────────┼───────────────┐
          ▼               ▼               ▼
   src/io            src/processing    src/statistics
   Reader/Writer     Pipeline          Calculator/Writer
   (gzip, batch)     (backend, rules)  (Q20/Q30, GC, length)
          │               │               │
          └───────────────┼───────────────┘
                          ▼
                   include/fqtools/  ← public Façade (fq.h)
```

The hot path runs behind a private `ExecutionBackend` seam. Its default oneTBB implementation is a three-stage `parallel_pipeline`:

```
serial_in_order (read batch)  →  parallel (filter/trim/stat)  →  serial_in_order (write + reduce)
```

- **Stage 1 — serial in-order.** gzip streams are sequential; reads come out in file order.
- **Stage 2 — parallel.** The CPU-bound work (predicates, mutators, per-base statistics) scales across cores. Batches are independent.
- **Stage 3 — serial in-order.** Output FASTQ stays ordered; statistics reduce deterministically by batch.

In-flight batch count is bounded by `maxLiveTokens`, so memory peaks are predictable. Batches come from an `ObjectPool`, so the hot path does not allocate per-batch.

The scheduling framework does not leak into the public API: v4 ships a sequential baseline and oneTBB as the production backend. Historical Taskflow comparisons remain in the performance archive only.

Three design decisions worth calling out:

1. **`FastqRecord` is all `string_view`.** Lifetime is bound to its `FastqBatch`; records cannot escape a batch. This is a deliberate constraint that makes zero-copy safe.
2. **`FastqBatch` owns one contiguous buffer.** `clear()` keeps capacity and recycles memory for the next batch — no `malloc/free` churn.
3. **One `interfaces.h` per domain.** No fine-grained interface sprawl; dependency injection still works for testing.

Full rationale (the *why*, not just the *what*) is in [docs/architecture.md](./docs/architecture.md).

## Representative performance

The maintained v4 baseline uses **1M reads × 150 bp**, fixed `seed=42`, five repetitions, and production Reader/Writer/Pipeline/StatisticWorker paths. It covers plain output, gzip levels 1/6/9, and single/batch writer APIs. See the [raw JSON and median/CV summary](./docs/performance/benchmark-reports/v4-baseline/2026-07-17/summary.md) for the exact environment and commands.

| Workload | v4 baseline |
| --- | --- |
| FASTQ reader | 202,903 reads/s (61.3 MiB/s) |
| Plain writer, single API | 117,444 reads/s (35.5 MiB/s) |
| gzip-6 writer, single API | 7,194 reads/s (2.2 MiB/s) |
| Filter baseline | 125,623 reads/s (38.0 MiB/s) |

**How to read these numbers.**

- These numbers are a same-machine reference, not a cross-environment promise. The WSL2 snapshot has inflated `real_time` values; compare only with the same command and environment.
- gzip level and single/batch API materially change Writer throughput, so one “Writer speed” number is not representative.
- The statistics run is marked high-CV in the current WSL2 snapshot; no optimization decision is made from that unstable timing.

**Comparison context.** Direct head-to-head numbers against fastp/seqkit are environment- and flag-sensitive, so they are not included in this table. The benchmark suite in `tools/benchmark/` (Google Benchmark) is reproducible; for running it and adding a comparison row on your hardware, see [docs/benchmark.md](./docs/benchmark.md).

## Quality bar

CI (`.github/workflows/ci.yml`) is manually triggered from the GitHub Actions UI; each run executes:

| Job | What it catches |
| --- | --- |
| **Format** | clang-format drift |
| **Static Analysis** | clang-tidy + cppcheck: API misuse, modern-C++ anti-patterns |
| **GCC Release** | Portability across compilers |
| **Clang Release** | Primary production toolchain |
| **Clang ASan** | Out-of-bounds, use-after-free, leaks |
| **Clang TSan** | Data races in the TBB pipeline |
| **Clang UBSan** | Undefined behavior |
| **Coverage** | Line coverage via gcovr + Codecov upload |

Fuzzer targets in `tools/fuzz/` exercise the FASTQ parser entry — the only place that consumes untrusted external input. They are not yet wired into CI; run them locally with a libFuzzer build. Tests are layered: unit (mirror `src/`), integration (cross-module), and e2e (CLI black-box via Bash + Python).

## Documentation map

| If you want to… | Start here |
| --- | --- |
| Build FastQTools and run your first command | [docs/getting-started.md](./docs/getting-started.md) |
| Check command syntax and options | [docs/cli-reference.md](./docs/cli-reference.md) |
| Integrate the library into C++ code | [docs/api.md](./docs/api.md) |
| Understand the architecture and design decisions | [docs/architecture.md](./docs/architecture.md) |
| Understand benchmark numbers | [docs/benchmark.md](./docs/benchmark.md) |
| Inspect performance data & optimization history | [docs/performance/README.md](./docs/performance/README.md) |
| Review incident postmortems & root-cause analyses | [docs/postmortems/README.md](./docs/postmortems/README.md) |
| Contribute docs or code | [CONTRIBUTING.md](./CONTRIBUTING.md) |
| Follow project-level changes | [CHANGELOG.md](./CHANGELOG.md) |

## Build requirements

- C++23-compatible compiler (**GCC 13+** or **Clang 17+** minimum; CI runs GCC 13 — Ubuntu runner default — and Clang 21)
- **CMake 3.28+**
- **Conan 2.x**
- Linux and macOS are supported build environments today; on Windows, use Docker or WSL.

## Contributing

FastQTools welcomes focused improvements: bug reports, docs fixes, tests, benchmark work, and narrow feature changes.

- Read the [contributing guide](./CONTRIBUTING.md)
- Open issues via [GitHub Issues](https://github.com/LessUp/fastq-tools/issues)
- Discuss ideas in [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions)

## License

FastQTools is released under the [MIT License](LICENSE). The technical narrative — why each decision was made — is in [docs/architecture.md](./docs/architecture.md).

## Author

**shijiashuai** — [GitHub](https://github.com/LessUp) · jiashuai.shi@qq.com
