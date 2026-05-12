---
layout: home

hero:
  name: FastQTools
  text: High-Performance FASTQ Toolkit
  tagline: Process FASTQ files at 1.7M reads/sec with zero-copy design, specification-driven, easy to audit and integrate.
  actions:
    - theme: brand
      text: Get Started
      link: /en/guide/getting-started
    - theme: alt
      text: GitHub
      link: https://github.com/LessUp/fastq-tools
    - theme: alt
      text: Benchmarks
      link: /en/performance/benchmark-report
    - theme: alt
      text: API Docs
      link: /en/api/overview

features:
  - icon: ⚡
    title: Extreme Performance
    details: 1.7M reads/sec on standard hardware. Intel oneTBB lock-free parallel pipeline. Minimize allocations and page faults.
  - icon: 📖
    title: Zero-Copy Design
    details: All record processing uses std::string_view. Minimal overhead on large FASTQ files or streaming buffers.
  - icon: 📋
    title: Specification-Driven
    details: Every API decision and file format is documented in openspec/baseline. Easy to audit, predict, and integrate.
---

## What You Can Do

| Task | Description |
|------|-------------|
| **Inspect FASTQ quality in one pass** | `FastQTools stat` computes read counts, max read length, total bases, base composition, GC content, quality summaries, and optional lightweight QC sidecars in a single streaming pass. |
| **Filter and trim raw data** | `FastQTools filter` combines quality, length, and N-ratio thresholds, then trims low-quality ends in one tool without intermediate files. |
| **Embed in C++ pipelines** | Reuse public API zero-copy FASTQ primitives without reimplementing basic processing logic. |

## Architecture Overview

![FastQTools Architecture](/assets/images/architecture.svg)

The pipeline uses Intel oneTBB for lock-free parallelism, with zero-copy `std::string_view` throughout. Records flow from I/O through processing stages without allocation overhead.

## Performance Comparison

| Feature | FastQTools | seqtk | fastp |
|---------|------------|-------|-------|
| **Throughput** | 1.7M reads/s | ~500K reads/s | ~800K reads/s |
| **Memory model** | Zero-copy, bounded pool | Buffer-based | Adaptive |
| **Parallelism** | TBB pipeline (auto-scale) | Single-threaded | Multi-threaded |
| **QC sidecar** | ✅ Built-in | ❌ | ✅ HTML report |
| **Quality trimming** | ✅ AVX2 optimized | ✅ Basic | ✅ Built-in |
| **Adapter trimming** | ✅ | ❌ | ✅ Auto-detect |
| **Signature stats** | ✅ k-mer signatures | ❌ | ❌ |
| **C++ API** | ✅ Public headers | ❌ | ❌ |
| **Spec-driven** | ✅ Full baseline | ❌ | ❌ |

::: info Benchmark conditions
AMD Ryzen 9 5900X, 100K reads × 150bp, single-threaded read/write, parallel processing.
:::

## Quick Stats

| Operation | Speed | Cores |
|-----------|-------|-------|
| FASTQ read | 1696 MB/s | 1× |
| FASTQ write | 1.76M reads/s | 1× |
| Full stat pass | 302 MB/s | parallel |
| Combined filter | 1.67M reads/s | parallel |

See [full benchmarks](performance/benchmark-report) for methodology and hardware details.

## Choose Your Path

| I want to… | Start here |
|------------|------------|
| Build and run the first command | [Getting Started](guide/getting-started) |
| See command syntax and examples | [CLI Reference](guide/cli-reference) |
| Understand the benchmark methodology | [Benchmark Overview](performance/benchmark-report) |
| Integrate the library in C++ | [API Overview](api/overview) |
| Work on the project | [Developer Guide](dev/) |

## Why Specifications Matter

FastQTools is **specification-driven** from the ground up. Every public API, file format decision, and performance guarantee is documented in version-controlled specifications. This means:

- **Easy to audit**: No hidden behavior. Read [`openspec/baseline/api/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline/api) for the exact API contract.
- **Easy to extend**: Proposals go in [`openspec/changes/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/changes). Reviewers can see the impact before code is written.
- **Easy to maintain**: Archived decisions in [`openspec/archive/`](https://github.com/LessUp/fastq-tools/tree/master/openspec/archive). No re-discovering the "why" of old choices.
