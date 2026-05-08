# Product Specification: FASTQ Processing

> **Status**: Active
> **Last Updated**: 2026-04-28
> **Owner**: FastQTools Core Team

## Overview

FastQTools is a focused C++23 FASTQ preprocessing and quality-control toolkit for day-to-day sequencing workflows. The product surface remains intentionally small: a CLI for routine QC/preprocessing tasks and a reusable C++ API for embedding the same primitives into other tools.

The project keeps a narrow product scope, but the maintained baseline now allows additive evolution in execution policy, bounded preprocessing modules, and lightweight QC signatures as long as the default user experience remains stable.

## Target Users

- Bioinformatics researchers who need fast local FASTQ inspection and filtering
- Pipeline engineers who want a lightweight QC step instead of a workflow platform
- C++ developers who want reusable FASTQ I/O and processing primitives

## Core Product Surface

### 1. `stat` command

Generate a tabular quality-control report for one FASTQ file, with an optional lightweight signature sidecar.

**Current contract**

- Required inputs: `--input`, `--output`
- Runtime controls: threads, batch size, performance profile, memory limit, quality encoding
- Output: a text report with summary lines and per-position metrics; optional signature sidecar for duplicate estimation and head-kmer summaries
- Supported inputs: plain FASTQ and gzip-compressed FASTQ (`.gz`)

**Representative usage**

```bash
FastQTools stat -i input.fastq.gz -o sample.stat.txt
```

### 2. `filter` command

Filter and optionally trim reads in a single pass.

**Current contract**

- Required inputs: `--input`, `--output`
- Supported predicates: minimum average quality, minimum length, maximum length, maximum N ratio
- Supported mutators: quality trimming, adapter trimming, polyG tail trimming, bounded polyX tail trimming
- Runtime controls: threads, batch size, performance profile, memory limit, quality encoding
- Supported I/O: plain FASTQ and gzip-compressed FASTQ (`.gz`)

**Representative usage**

```bash
FastQTools filter \
  -i input.fastq.gz \
  -o output.fastq.gz \
  --min-quality 20 \
  --min-length 50 \
  --trim-quality 20 \
  --trim-mode both
```

## Key Capabilities

1. **Zero-copy FASTQ batches** — records are exposed as `std::string_view` into `FastqBatch` storage.
2. **Predicate/mutator pipeline** — filtering and trimming are composed through a processing pipeline interface.
3. **Per-position statistics** — `stat` computes summary metrics plus per-position base/quality distributions.
4. **Gzip-aware file handling** — the maintained compression path is gzip for both reading and writing.
5. **Execution and memory policy controls** — default oneTBB + object-pool execution remains explicit and configurable.
6. **Lightweight QC signatures** — `stat` may emit bounded sidecar summaries for duplicate estimation and head-kmer signatures without replacing the default text report.
7. **Embeddable C++ surface** — public headers under `include/fqtools/` remain the supported integration boundary.

## Explicit Non-Goals

- Becoming a general-purpose workflow runner
- Claiming maintained support for bzip2/xz I/O when the current implementation does not provide it
- Expanding the public surface with new commands instead of extending the existing `stat` / `filter` workflows
- Turning lightweight QC signatures into a classifier, mapper, or platform service
- Keeping legacy docs, workflows, or metadata that no longer improve the maintained product

## Representative Performance Targets

FastQTools keeps a compact benchmark posture for routine QC workloads. Current representative targets remain:

- FASTQ read path: **>= 1600 MB/s**
- FASTQ write path: **>= 1.7M reads/s**
- Combined filtering pass: **>= 1.6M reads/s**
- Statistics pass: **>= 300 MB/s**

These are product-sizing targets for the maintained benchmark dataset, not universal guarantees for every input or environment.

## Acceptance Criteria

1. `stat` and `filter` must work on plain FASTQ and `.gz` FASTQ inputs.
2. `filter` must preserve record integrity (`seq` and `qual` lengths remain aligned after mutation).
3. `filter` must continue to work when adapter trimming or poly-tail trimming are enabled.
4. `stat` output must include summary metrics and a per-position table.
5. Optional signature sidecar output must remain additive and must not replace the default text report.
6. Public C++ integration continues to use headers in `include/fqtools/` as the supported boundary.
7. Build, test, and lint entry points remain the scripts under `scripts/core/`.

## Related Specifications

- [Core Architecture](../architecture/0001-core-architecture.md)
- [Toolchain Policy](../architecture/0002-toolchain-policy.md)
- [Core API](../api/core-api.md)
- [Schema Specification](../schemas/schema.md)
