# CLI Reference

FastQTools provides two subcommands — `stat` and `filter` — for FASTQ file statistical analysis and quality filtering.

---

## Basic Syntax

```bash
FastQTools [global options] <subcommand> [subcommand options]
```

> Global options must be placed before the subcommand.

---

## Global Options

| Option | Description |
|--------|-------------|
| `-v, --verbose` | Enable debug logging (equivalent to `--log-level=debug`) |
| `-q, --quiet` | Errors only (equivalent to `--log-level=error`) |
| `--log-level=LEVEL` | Set log level: `trace`, `debug`, `info`, `warn`, `error` |
| `--help` | Show help message |

Default log level is `info`.

---

## stat Command — Statistical Analysis

Performs comprehensive quality statistics on FASTQ files.

### Usage

```bash
FastQTools stat -i <input> -o <output> [options]
```

### Options

| Option | Description |
|--------|-------------|
| `-i, --input <path>` | Input FASTQ file (supports `.gz` compression) |
| `-o, --output <path>` | Output statistics file |
| `-t, --threads <N>` | Number of threads (auto-detect by default) |
| `--batch-size <N>` | Number of reads per batch |

### Output Metrics

- **Read statistics**: Total reads, valid reads
- **Length distribution**: Min/max/average length
- **Quality analysis**: Q20/Q30 base percentages
- **Base composition**: A/T/C/G/N ratios
- **GC content**: Overall and position-specific

### Examples

```bash
# Basic statistics
FastQTools stat -i reads.fq.gz -o analysis.txt

# Multi-threaded processing
FastQTools stat -i reads.fq.gz -o analysis.txt -t 8

# Debug mode
FastQTools -v stat -i reads.fq.gz -o analysis.txt
```

---

## filter Command — Filtering & Trimming

Clean, filter, and quality-trim raw sequencing data.

### Usage

```bash
FastQTools filter -i <input> -o <output> [options]
```

### Filter Options

| Option | Description |
|--------|-------------|
| `-i, --input <path>` | Input FASTQ file (supports `.gz`) |
| `-o, --output <path>` | Output FASTQ file (`.gz` suffix auto-compresses) |
| `-t, --threads <N>` | Number of threads |
| `--min-quality <float>` | Minimum average quality threshold |
| `--min-length <int>` | Minimum read length |
| `--max-length <int>` | Maximum read length |
| `--max-n-ratio <0.0-1.0>` | Maximum N-base ratio |

### Trimming Options

| Option | Description |
|--------|-------------|
| `--trim-quality <float>` | Quality trimming threshold |
| `--trim-mode <mode>` | Trimming mode: `both` (both ends), `five` (5' end), `three` (3' end) |

### Examples

```bash
# Quality filtering
FastQTools filter -i input.fq.gz -o filtered.fq.gz \
  --min-quality 20 --min-length 50

# Quality trimming (remove low-quality bases from 3' end)
FastQTools filter -i input.fq.gz -o trimmed.fq.gz \
  --trim-quality 20 --trim-mode three

# Combined filtering + trimming
FastQTools filter -i input.fq.gz -o clean.fq.gz \
  --min-quality 20 --min-length 50 --max-n-ratio 0.1 \
  --trim-quality 20 --trim-mode both

# Quiet mode
FastQTools -q filter -i input.fq.gz -o filtered.fq.gz --min-quality 20
```

---

## Exit Codes

| Exit Code | Meaning |
|-----------|---------|
| `0` | Success |
| Non-`0` | Error (unknown subcommand, argument error, runtime exception, etc.) |

---

## C++ API Integration

FastQTools can also be used as a C++ library:

```cpp
#include <fqtools/fq.h>

// Statistical analysis
fq::statistic::StatisticOptions options;
options.inputFastqPath = "input.fastq.gz";
options.outputStatPath = "output.stat.txt";
auto calculator = fq::statistic::createStatisticCalculator(options);
calculator->run();

// Filtering
auto pipeline = fq::processing::createProcessingPipeline();
pipeline->setInputPath("input.fq.gz");
pipeline->setOutputPath("filtered.fq.gz");

fq::processing::ProcessingConfig config;
config.threadCount = 4;
pipeline->setProcessingConfig(config);

auto stats = pipeline->run();
```

See [API Reference](../api/overview.md) for full API documentation.
