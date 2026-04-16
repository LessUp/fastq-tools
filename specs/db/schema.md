# Schema Specification: Configuration & Data Models

> **Status**: Active  
> **Last Updated**: 2026-04-17  
> **Related**: [Product Spec](../product/fastq-processing.md)

## Overview

This document defines the configuration schemas and data models used throughout FastQTools. Since FastQTools is a CLI tool rather than a database-backed service, this focuses on configuration formats and output schemas.

## Configuration Schema

### Pipeline Configuration

```json
{
  "pipeline": {
    "thread_count": 4,
    "batch_size": 10000,
    "buffer_size": 1048576,
    "compression_level": 6
  }
}
```

**Field Definitions**:

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `thread_count` | integer | No | CPU cores | Number of parallel threads |
| `batch_size` | integer | No | 10000 | Records per batch |
| `buffer_size` | integer | No | 1MB | I/O buffer size in bytes |
| `compression_level` | integer | No | 6 | Gzip compression level (1-9) |

### Filter Configuration

```json
{
  "filter": {
    "min_quality": 20,
    "max_quality": 40,
    "min_length": 50,
    "max_length": 300,
    "max_n_ratio": 0.05,
    "trim_quality": null,
    "trim_mode": "both_ends"
  }
}
```

**Field Definitions**:

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `min_quality` | integer | No | 0 | Minimum quality score |
| `max_quality` | integer | No | 40 | Maximum quality score |
| `min_length` | integer | No | 0 | Minimum read length |
| `max_length` | integer | No | ∞ | Maximum read length |
| `max_n_ratio` | float | No | 1.0 | Maximum N-base ratio |
| `trim_quality` | integer | No | null | Quality trimming threshold |
| `trim_mode` | string | No | null | Trimming mode: `both_ends`, `5prime`, `3prime` |

### Statistics Configuration

```json
{
  "statistics": {
    "output_format": "text",
    "include_length_distribution": true,
    "include_base_composition": true,
    "include_quality_distribution": false,
    "bin_count": 100
  }
}
```

## Output Schemas

### Statistics Output

```json
{
  "statistics": {
    "total_reads": 100000,
    "total_bases": 15000000,
    "q20_count": 95000,
    "q30_count": 85000,
    "gc_content": 0.45,
    "at_content": 0.54,
    "n_content": 0.01,
    "base_composition": {
      "A": 0.27,
      "T": 0.27,
      "C": 0.23,
      "G": 0.22,
      "N": 0.01
    },
    "length_distribution": {
      "100": 10000,
      "150": 80000,
      "200": 10000
    },
    "quality_distribution": {
      "0-10": 100,
      "10-20": 500,
      "20-30": 10000,
      "30-40": 89400
    }
  }
}
```

### Filter Output Summary

```json
{
  "filter_summary": {
    "input_reads": 100000,
    "output_reads": 85000,
    "pass_rate": 0.85,
    "filtered_by_quality": 10000,
    "filtered_by_length": 3000,
    "filtered_by_n_ratio": 2000,
    "trimmed_reads": 5000
  }
}
```

## CLI Argument Schema

### Global Options

```
FastQTools [OPTIONS] <command> [command_options]

Global Options:
  -h, --help          Show help
  -v, --version       Show version
  --log-level LEVEL   Set log level (trace/debug/info/warn/error/critical)
  --config FILE       Load configuration from FILE
```

### Stat Command

```
FastQTools stat [OPTIONS]

Options:
  -i, --input FILE      Input FASTQ file (required)
  -o, --output FILE     Output report file (required)
  --format FORMAT       Output format: text, json (default: text)
  --no-length-dist      Skip length distribution
  --no-base-composition Skip base composition
```

### Filter Command

```
FastQTools filter [OPTIONS]

Options:
  -i, --input FILE       Input FASTQ file (required)
  -o, --output FILE      Output FASTQ file (required)
  --min-quality N        Minimum quality threshold
  --max-quality N        Maximum quality threshold
  --min-length N         Minimum length filter
  --max-length N         Maximum length filter
  --max-n-ratio RATIO    Maximum N-base ratio
  --trim-quality N       Quality trimming threshold
  --trim-mode MODE       Trimming mode (both_ends/5prime/3prime)
  --report FILE          Write filter summary report
```

## Data Models

### CompressionType

```cpp
enum class CompressionType {
    None,
    Gzip,
    Bzip2,
    Xz
};
```

### StatisticType

```cpp
enum class StatisticType {
    Basic,
    Full,
    GcContent,
    QualityDistribution,
    LengthDistribution
};
```

### PipelineStage

```cpp
enum class PipelineStage {
    Reader,
    Filtering,
    Trimming,
    Statistics,
    Writer
};
```

## Validation Rules

### Configuration Validation

1. `thread_count` must be in range [1, 256]
2. `batch_size` must be in range [100, 1000000]
3. `min_quality` must be in range [0, 40]
4. `min_length` must be < `max_length` (if both specified)
5. `max_n_ratio` must be in range [0.0, 1.0]
6. File paths must be valid and accessible (for input) or creatable (for output)

### Error Messages

All validation errors use `fq::error::ConfigurationError`:

```cpp
FQ_THROW_CONFIG_ERROR("thread_count must be between 1 and 256, got: " + std::to_threads(value))
```

## Related Documents

- [Product Specification](../product/fastq-processing.md)
- [API Specification](core-api.md)
- [CLI Reference](../../docs/guide/cli-reference.md)
