# Product Specification: FASTQ Processing

> **Status**: Active
> **Last Updated**: 2026-04-26
> **Owner**: FastQTools Core Team

## Overview

FastQTools is a focused C++23 FASTQ quality-control toolkit for day-to-day sequencing workflows. The product surface is intentionally small: a CLI for routine QC tasks and a reusable C++ API for embedding the same primitives into other tools.

The project is in a maintenance closeout phase. Product scope is therefore defined by what is already implemented, maintained, and worth keeping trustworthy over time.

## Target Users

- Bioinformatics researchers who need fast local FASTQ inspection and filtering
- Pipeline engineers who want a lightweight QC step instead of a workflow platform
- C++ developers who want reusable FASTQ I/O and processing primitives

## Core Product Surface

### 1. `stat` command

Generate a tabular quality-control report for one FASTQ file.

**Current contract**

- Required inputs: `--input`, `--output`
- Runtime controls: threads, batch size, memory limit, chunk size, quality encoding
- Output: a text report with summary lines and per-position metrics
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
- Supported mutators: quality trimming
- Runtime controls: threads, batch size, memory limit, buffer sizing, quality encoding
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
5. **Embeddable C++ surface** — public headers under `include/fqtools/` remain the supported integration boundary.

## Explicit Non-Goals

- Becoming a general-purpose workflow runner
- Claiming maintained support for bzip2/xz I/O when the current implementation does not provide it
- Expanding the public surface with new commands during the closeout phase
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
3. `stat` output must include summary metrics and a per-position table.
4. Public C++ integration continues to use headers in `include/fqtools/` as the supported boundary.
5. Build, test, and lint entry points remain the scripts under `scripts/core/`.

## Related Specifications

- [Core Architecture](../architecture/0001-core-architecture.md)
- [Toolchain Policy](../architecture/0002-toolchain-policy.md)
- [Core API](../api/core-api.md)
- [Schema Specification](../schemas/schema.md)
