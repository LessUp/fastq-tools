# Schema Specification: Configuration & Data Models

> **Status**: Active
> **Last Updated**: 2026-04-28
> **Related**: [Product Spec](../product/fastq-processing.md), [Core API](../api/core-api.md)

## Overview

FastQTools is a CLI-first local tool, not a service with a large external data contract. The maintained schemas are therefore the runtime configuration structs, command-line options, and the current `stat` report format emitted by the implementation.

This document intentionally avoids promising imaginary JSON contracts or compression modes that are not present in the codebase.

## Core Runtime Data Models

### `fq::config::ConfigValue`

Generic scalar configuration value used by `fq::config::Configuration`:

```cpp
using ConfigValue = std::variant<bool, std::int64_t, double, std::string>;
```

### Reader options

```cpp
struct FastqReaderOptions {
    size_t readChunkBytes = 1 * 1024 * 1024;
    size_t zlibBufferBytes = 128 * 1024;
    size_t maxBufferBytes = 0;
};
```

### Writer options

```cpp
enum class FastqWriterCompressionMode : std::uint8_t {
    Auto,
    Gzip,
    None,
};

struct FastqWriterOptions {
    size_t zlibBufferBytes = 128 * 1024;
    size_t outputBufferBytes = 128 * 1024;
    FastqWriterCompressionMode compression = FastqWriterCompressionMode::Auto;
};
```

### Processing configuration

```cpp
struct ProcessingConfig {
    size_t batchSize = 10000;
    size_t threadCount = 1;
    ExecutionBackend executionBackend = ExecutionBackend::OneTbb;
    MemoryResourcePolicy memoryResourcePolicy = MemoryResourcePolicy::ObjectPool;
    bool allocationTelemetryEnabled = false;
    size_t readChunkBytes = 1 * 1024 * 1024;
    size_t zlibBufferBytes = 128 * 1024;
    size_t writerBufferBytes = 128 * 1024;
    size_t batchCapacityBytes = 4 * 1024 * 1024;
    size_t memoryLimitBytes = 0;
    size_t maxInFlightBatches = 0;
};
```

### Processing result summary

```cpp
struct ProcessingStatistics {
    uint64_t totalReads = 0;
    uint64_t passedReads = 0;
    uint64_t filteredReads = 0;
    uint64_t modifiedReads = 0;
    uint64_t errorReads = 0;
    uint64_t inputBytes = 0;
    uint64_t outputBytes = 0;
    uint64_t elapsedMs = 0;
    double processingTimeMs = 0.0;
    double throughputMbps = 0.0;
    bool allocationTelemetryEnabled = false;
    MemoryResourcePolicy memoryResourcePolicy = MemoryResourcePolicy::ObjectPool;
    size_t resolvedMaxInFlightBatches = 0;
};
```

### Statistics configuration

```cpp
struct StatisticOptions {
    std::string inputFastqPath;
    std::string outputStatPath;
    std::string signatureReportPath;
    uint32_t batchSize = 50000;
    size_t signatureKmerSize = 15;
    size_t maxReportedSignatures = 20;
    size_t duplicateEstimateSampleModulo = 1024;
    uint32_t threadCount = 4;
    fq::processing::ExecutionBackend executionBackend = fq::processing::ExecutionBackend::OneTbb;
    fq::processing::MemoryResourcePolicy memoryResourcePolicy =
        fq::processing::MemoryResourcePolicy::ObjectPool;
    bool allocationTelemetryEnabled = false;
    size_t readChunkBytes = 1 * 1024 * 1024;
    size_t zlibBufferBytes = 128 * 1024;
    size_t batchCapacityBytes = 4 * 1024 * 1024;
    size_t memoryLimitBytes = 0;
    size_t maxInFlightBatches = 0;
    int qualityEncoding = 33;
};
```

## CLI Argument Schema

## Global CLI

```text
FastQTools [global-options] <command> [options]

Global options:
  -v, --verbose
  -q, --quiet
  --log-level=LEVEL
  -h, --help
```

**Supported commands**: `stat`, `filter`

## `stat` command

```text
FastQTools stat [options]

Required:
  -i, --input FILE
  -o, --output FILE

Runtime:
  -t, --threads N
  --batch-size N
  --profile {default|lowMemory|highThroughput}
  --memory-limit-gb N
  --quality-encoding {33|64}

 Optional sidecar:
  --signature-report FILE
  --signature-kmer-size N
  --signature-limit N
  --duplicate-sample-modulo N
  -h, --help
```

## `filter` command

```text
FastQTools filter [options]

Required:
  -i, --input FILE
  -o, --output FILE

Runtime:
  -t, --threads N
  --batch-size N
  --profile {default|lowMemory|highThroughput}
  --memory-limit-gb N
  --quality-encoding {33|64}

Predicates:
  --min-quality FLOAT
  --min-length N
  --max-length N
  --max-n-ratio FLOAT

Mutators:
  --trim-quality FLOAT
  --trim-mode {both|five|three}
  --adapter-seq SEQ (repeatable)
  --adapter-min-overlap N
  --adapter-max-mismatches N
  --trim-poly-g N
  --trim-poly-x N

Other:
  -h, --help
```

## `stat` Output File Schema

The maintained `stat` output is a text report written by `FastqStatisticCalculator::writeResult(...)`.

### Summary header lines

The output begins with comment-style summary lines:

```text
#Name    <input name>
#PhredQual    33
#ReadNum    <count>
#DuplicateEstimate    <count>
#DuplicateEstimateRate    <percent>%
#MaxReadLength    <count>
#BaseCount    <count>
#Q20(>=20)    <count>    <percent>%
#Q30(>=30)    <count>    <percent>%
#A    <count>    <percent>%
#C    <count>    <percent>%
#G    <count>    <percent>%
#T    <count>    <percent>%
#N    <count>    <percent>%
#GC   <count>    <percent>%
```

### Per-position table

After the summary, the report contains:

```text
#Pos    A    C    G    T    N    AvgQual    ErrRate
1       ...
2       ...
...
```

Each row corresponds to one read position and contains:

1. 1-based position index
2. per-base counts (`A`, `C`, `G`, `T`, `N`)
3. average quality at that position
4. estimated error rate at that position

## Optional Signature Sidecar Schema

When `--signature-report` is supplied, `stat` may emit a TSV sidecar with the following shape:

```text
metric   key                 count
summary  total_reads         <count>
summary  duplicate_estimate  <count>
head_kmer <kmer>             <count>
```

The sidecar is additive and bounded: `head_kmer` rows are limited by `--signature-limit`, and duplicate estimation is derived from hashed sampling controlled by `--duplicate-sample-modulo`.

## Validation Rules

1. `FastqRecord::validateLengths()` must remain true for valid processed records.
2. `threadCount` must be positive when supplied through CLI or runtime config.
3. `qualityEncoding` is currently expected to be `33` or `64`.
4. `trim-mode` is currently one of `both`, `five`, or `three`.
5. `profile` currently accepts `default`, `lowMemory`, and `highThroughput`.
6. `memory-limit-gb` remains an optional override layered on top of the selected profile.
7. `duplicate-sample-modulo` must be a positive integer.
8. Supported maintained compression mode is gzip; no schema contract is made for bzip2/xz.

## Notes on Scope

- This document does **not** define a JSON API contract for `stat`; the maintained implementation writes a tabular text report.
- This document does **not** promise a general configuration-file schema beyond the scalar key-value model exposed by `fq::config::Configuration`.

## Related Specifications

- [Product Specification](../product/fastq-processing.md)
- [Core API](../api/core-api.md)
