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
| `-t, --threads <N>` | Number of threads (default `1`) |
| `--batch-size <N>` | Number of reads per batch |
| `--execution-backend oneTbb` | Explicit execution backend selection |
| `--memory-policy objectPool` | Explicit memory resource policy |
| `--allocation-telemetry` | Emit memory policy and resolved in-flight metadata |
| `--signature-report <path>` | Write an optional signature sidecar (TSV) |
| `--signature-kmer-size <N>` | Head-kmer size used in the sidecar |
| `--signature-limit <N>` | Maximum number of signature rows in the sidecar |
| `--duplicate-sample-modulo <N>` | Sampling modulo for duplicate estimation; default `1024`, and `1` is useful for exact tests |

### Output Metrics

- **Read statistics**: Total reads, max read length, and total bases
- **Quality analysis**: Q20/Q30 base percentages
- **Base composition**: A/T/C/G/N ratios
- **GC content**: Overall and position-specific
- **Per-position table**: Base counts, average quality, and estimated error rate for each cycle
- **Lightweight sidecar**: Optional duplicate estimate and top head-kmer signatures

### Examples

```bash
# Basic statistics
FastQTools stat -i reads.fq.gz -o analysis.txt

# Multi-threaded processing
FastQTools stat -i reads.fq.gz -o analysis.txt -t 8

# Emit a signature sidecar
FastQTools stat -i reads.fq.gz -o analysis.txt \
  --signature-report signatures.tsv --signature-kmer-size 15

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
| `--adapter-seq <seq>` | 3' adapter sequence, repeatable |
| `--adapter-min-overlap <N>` | Minimum overlap for adapter trimming |
| `--adapter-max-mismatches <N>` | Maximum mismatches allowed for adapter trimming |
| `--trim-poly-g <N>` | Trim polyG tails with run length >= `N` |
| `--trim-poly-x <N>` | Trim low-complexity polyX tails with run length >= `N` |

### Runtime Options

| Option | Description |
|--------|-------------|
| `--execution-backend oneTbb` | Explicit execution backend selection |
| `--memory-policy objectPool` | Explicit memory resource policy |
| `--allocation-telemetry` | Include resolved memory telemetry in processing stats |

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

# Adapter + poly-tail preprocessing
FastQTools filter -i input.fq.gz -o clean.fq.gz \
  --adapter-seq AGATCGGAAGAGC --adapter-min-overlap 6 \
  --trim-poly-g 8 --trim-poly-x 8

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
options.signatureReportPath = "output.signatures.tsv";
auto calculator = fq::statistic::createStatisticCalculator(options);
calculator->run();

// Filtering
auto pipeline = fq::processing::createProcessingPipeline();
pipeline->setInputPath("input.fq.gz");
pipeline->setOutputPath("filtered.fq.gz");

fq::processing::ProcessingConfig config;
config.threadCount = 4;
config.executionBackend = fq::processing::ExecutionBackend::OneTbb;
pipeline->setProcessingConfig(config);

auto stats = pipeline->run();
```

See [API Reference](../api/overview.md) for full API documentation.
