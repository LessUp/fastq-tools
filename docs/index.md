---
hide:
  - navigation
  - toc
---

<div class="hero-section" markdown>

<p class="hero-eyebrow">Focused FASTQ quality-control toolkit</p>

# FastQTools

<p class="lead">Fast statistics and filtering for FASTQ files, with a CLI for routine QC work and a C++ API for pipeline integration.</p>

<div class="hero-actions" markdown>
[Get started](guide/getting-started.en.md){ .md-button .md-button--primary }
[CLI reference](guide/cli-reference.en.md){ .md-button }
[Benchmarks](performance/benchmark-report.md){ .md-button }
[View on GitHub](https://github.com/LessUp/fastq-tools){ .md-button }
</div>

<p class="hero-note">Built for researchers and pipeline engineers who want a focused FASTQ tool, not a heavyweight workflow platform.</p>

</div>

## What you can do with it

<div class="grid cards" markdown>

-   :material-chart-box: **Inspect sequencing runs quickly**

    ---

    Use `stat` for read counts, length distribution, base composition, GC content, and quality summaries in one pass.

-   :material-filter-outline: **Filter and trim before downstream steps**

    ---

    Use `filter` to combine quality, length, and N-ratio thresholds, then trim low-quality ends without stitching together separate tools.

-   :material-code-braces: **Embed FASTQ processing in C++**

    ---

    Reuse the public API when you need the same FASTQ primitives inside a larger library or production pipeline.

</div>

## Start here

| If you are trying to… | Go to |
| --- | --- |
| Build FastQTools and run the first command | [Getting Started](guide/getting-started.en.md) |
| Check command syntax and examples | [CLI Reference](guide/cli-reference.en.md) |
| Understand the benchmark numbers | [Benchmark Overview](performance/benchmark-report.md) |
| Integrate the library from C++ | [API Overview](api/overview.en.md) |
| Build or test the project itself | [Developer Guide](dev/index.en.md) |
| Contribute docs or code | [Contributing](contributing.en.md) |

## Representative benchmarks

FastQTools publishes a compact benchmark snapshot so new users can judge whether the tool fits everyday QC workloads.

| Workload | Representative result |
| --- | --- |
| FASTQ read path | 1696 MB/s |
| FASTQ write path | 1.76M reads/s |
| Combined filtering pass | 1.67M reads/s |
| Full statistics pass | 302 MB/s |

<p class="muted-note">These figures come from the maintained 100K-read (150 bp) benchmark set on an AMD Ryzen 9 5900X. They are intended as context, not universal promises. See the full <a href="performance/benchmark-report/">benchmark overview</a> for methodology and caveats.</p>

## Documentation map

<div class="grid cards" markdown>

-   **Start using the CLI**

    ---

    [Getting Started](guide/getting-started.en.md)<br>
    [CLI Reference](guide/cli-reference.en.md)

-   **Integrate the library**

    ---

    [API Overview](api/overview.en.md)<br>
    [IO Module](api/io.en.md)

-   **Work on the project**

    ---

    [Developer Guide](dev/index.en.md)<br>
    [Contributing](contributing.en.md)

-   **Track project movement**

    ---

    [Changelog](changelog.en.md)<br>
    [GitHub Releases](https://github.com/LessUp/fastq-tools/releases)

</div>

---

<p style="text-align: center; color: var(--md-default-fg-color--light);">
  <a href="index.en.md">中文概览</a> ·
  <a href="https://github.com/LessUp/fastq-tools">GitHub</a> ·
  MIT License
</p>
