---
layout: home
---

<div class="home-header">
  <div class="home-header-left">
    <div class="home-logo">FQ</div>
    <div>
      <span class="home-title">FastQTools</span>
      <span class="home-subtitle">High-Performance FASTQ Toolkit</span>
    </div>
  </div>
  <div class="home-nav">
    <a href="./guide/getting-started">Guide</a>
    <a href="https://github.com/LessUp/fastq-tools">GitHub</a>
    <a href="../zh/">中文</a>
  </div>
</div>

<div class="home-intro-row">
  <div class="home-intro">
    High-performance FASTQ processing CLI toolkit for bioinformatics QC workflows. Process FASTQ files at 1.7M reads/sec on standard hardware, with zero-copy design, specification-driven, easy to audit and integrate.
  </div>
  <div class="home-stats">
    <span><strong>C++23</strong> native</span>
    <span><strong>Zero-copy</strong> design</span>
    <span><strong>TBB</strong> parallel</span>
  </div>
</div>

## Features

<div class="feature-map">
  <div class="feature-card">
    <div class="feature-card-title">⚡ Extreme Performance</div>
    <div class="feature-card-desc">
      1.7M reads/sec on standard hardware. Intel oneTBB lock-free parallel pipeline. Minimize allocations and page faults.
    </div>
    <div class="feature-tags">
      <a href="./performance/benchmark-report" class="feature-tag">Benchmarks</a>
      <a href="./dev/architecture" class="feature-tag">Architecture</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">📖 Zero-Copy Design</div>
    <div class="feature-card-desc">
      All record processing uses std::string_view. Minimal overhead on large FASTQ files or streaming buffers.
    </div>
    <div class="feature-tags">
      <a href="./api/overview" class="feature-tag">API</a>
      <a href="./dev/design" class="feature-tag">Design</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">📋 Specification-Driven</div>
    <div class="feature-card-desc">
      Every API decision and file format is documented in openspec/baseline. Easy to audit, predict, and integrate.
    </div>
    <div class="feature-tags">
      <a href="https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline" class="feature-tag">Specs</a>
      <a href="https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline/architecture" class="feature-tag">ADRs</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">📊 One-Pass QC Stats</div>
    <div class="feature-card-desc">
      <code>FastQTools stat</code> computes read counts, base composition, GC content, quality summaries, and optional lightweight QC sidecars in a single streaming pass.
    </div>
    <div class="feature-tags">
      <a href="./guide/cli-reference" class="feature-tag">CLI Reference</a>
      <a href="./guide/getting-started" class="feature-tag">Getting Started</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🔧 Flexible Filtering</div>
    <div class="feature-card-desc">
      <code>FastQTools filter</code> combines quality, length, and N-ratio thresholds, then trims low-quality ends in one tool without intermediate files.
    </div>
    <div class="feature-tags">
      <a href="./guide/configuration" class="feature-tag">Configuration</a>
      <a href="./guide/cli-reference" class="feature-tag">Usage</a>
    </div>
  </div>

  <div class="feature-card">
    <div class="feature-card-title">🚀 C++ Library Integration</div>
    <div class="feature-card-desc">
      Reuse public API zero-copy FASTQ primitives without reimplementing basic processing logic.
    </div>
    <div class="feature-tags">
      <a href="./api/overview" class="feature-tag">API</a>
      <a href="./guide/deployment" class="feature-tag">Integration</a>
    </div>
  </div>
</div>

## Performance Comparison

| Feature | FastQTools | seqtk | fastp |
|---------|------------|-------|-------|
| **Throughput** | 1.7M reads/s | ~500K reads/s | ~800K reads/s |
| **Memory model** | Zero-copy, bounded pool | Buffer-based | Adaptive |
| **Parallelism** | TBB pipeline (auto-scale) | Single-threaded | Multi-threaded |
| **QC sidecar** | ✅ Built-in | ❌ | ✅ HTML report |
| **Quality trimming** | ✅ AVX2 optimized | ✅ Basic | ✅ Built-in |
| **C++ API** | ✅ Public headers | ❌ | ❌ |
| **Spec-driven** | ✅ Full baseline | ❌ | ❌ |

::: info Benchmark conditions
AMD Ryzen 9 5900X, 100K reads × 150bp, single-threaded read/write, parallel processing.
:::

<div class="quick-start">
  <div class="quick-start-title">Quick Start</div>
  <div class="quick-start-content">
    <div class="command-block">
      <code>./scripts/core/build && ./build/fastq-tools stat input.fastq</code>
    </div>
    After building, run <code>fastq-tools stat</code> or <code>fastq-tools filter</code>. See the <a href="./guide/getting-started">Getting Started Guide</a> for details.
  </div>
</div>

## Choose Your Path

| I want to… | Start here |
|------------|------------|
| Build and run the first command | [Getting Started](guide/getting-started) |
| See command syntax and examples | [CLI Reference](guide/cli-reference) |
| Understand the benchmark methodology | [Benchmark Overview](performance/benchmark-report) |
| Integrate the library in C++ | [API Overview](api/overview) |
| Work on the project | [Developer Guide](dev/) |
