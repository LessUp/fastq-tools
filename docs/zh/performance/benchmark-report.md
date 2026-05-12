# Benchmark Overview

FastQTools keeps a compact benchmark summary so users can estimate how the project fits common FASTQ QC workloads without reading raw CI artifacts first.

## Representative results

These results are the current maintained snapshot for **100K reads (150 bp)** on an **AMD Ryzen 9 5900X** using a Release build.

| Workload | Representative result | What it represents |
| --- | --- | --- |
| FASTQ read path | 1696 MB/s | Parsing and ingest throughput |
| FASTQ write path | 1.76M reads/s | Output-side throughput in the maintained benchmark set |
| Combined filtering pass | 1.67M reads/s | A realistic QC pass with multiple predicates enabled |
| Full statistics pass | 302 MB/s | End-to-end metrics collection |

## How to read these numbers

- They are **representative**, not guaranteed minima or maxima.
- Actual throughput depends on storage, compression level, thread count, filter mix, and read length distribution.
- The table is intentionally small: it is meant to answer “is this in the right performance range for my workload?” before you dive into deeper reports.

## Benchmark workload

- **Dataset shape**: synthetic FASTQ data, 100K reads, 150 bp per read
- **Environment**: Linux on AMD Ryzen 9 5900X
- **Build profile**: Release
- **Focus**: routine FASTQ QC tasks, not cross-tool marketing comparisons

## Benchmark artifacts and tooling

- This page is the stable, curated overview for public readers.
- CI-generated benchmark artifacts may be published under `docs/performance/benchmark-reports/` when available.
- Benchmark tooling lives in `scripts/tools/performance/` inside the repository.

## When benchmarks matter most

Benchmarks are most useful when you are deciding whether FastQTools is a good fit for:

1. pre-alignment filtering in existing QC pipelines,
2. repeated batch statistics over many FASTQ files, or
3. embedding FASTQ processing into a larger C++ application.

If you need command syntax next, go to the [CLI Reference](../guide/cli-reference.md). If you need integration details, go to the [API Overview](../api/overview.md).
