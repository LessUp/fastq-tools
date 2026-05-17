# CLI Reference

FastQTools exposes only a small set of core CLI concepts: global logging options, the `stat` subcommand, and the `filter` subcommand. The interface stays intentionally narrow so the command line remains scriptable, reproducible, and maintainable as the external boundary.

## Command structure

```bash
FastQTools [global options] <subcommand> [subcommand options]
```

One important rule is that **global options come before the subcommand**. If you need to adjust the log level, write it like this:

```bash
FastQTools --log-level debug stat -i reads.fastq.gz -o stats.txt
```

## Global options

| Option | Purpose | Typical use |
| --- | --- | --- |
| `-v`, `--verbose` | Increase log detail | Troubleshooting and local validation |
| `-q`, `--quiet` | Keep only error output | Batch jobs and CI |
| `--log-level <level>` | Set the log level explicitly | Stable automation and scripting |
| `--help` | Show help output | First use and parameter confirmation |

## `stat`: statistics and evidence building

`stat` does not modify the input. Its job is to interpret the data as comparable, traceable quality evidence.

```bash
FastQTools stat -i reads.fastq.gz -o stats.txt
FastQTools stat -i reads.fastq.gz -o stats.txt -t 8
FastQTools stat -i reads.fastq.gz -o stats.txt \
    --signature-report signatures.tsv \
    --signature-kmer-size 15
```

You would typically call it when you want to:

- inspect read length, GC, Q20/Q30, and quality distribution quickly;
- establish a baseline before filtering instead of tuning by guesswork;
- leave behind an auditable statistics output inside an automated workflow.

## `filter`: filtering, trimming, and preprocessing

`filter` turns policy into actual output. Its options are easiest to think about in three groups:

| Parameter group | Representative options | Meaning |
| --- | --- | --- |
| Input / output | `-i`, `-o`, `-t` | Select files and concurrency |
| Filtering rules | `--min-quality`, `--min-length`, `--max-n-ratio` | Define which reads are kept |
| Trimming rules | `--trim-quality`, `--trim-mode`, `--adapter-seq`, `--trim-poly-g`, `--trim-poly-x` | Define how retained reads are changed |

Example:

```bash
FastQTools filter -i reads.fastq.gz -o clean.fastq.gz \
    --min-quality 20 \
    --min-length 50 \
    --max-n-ratio 0.1 \
    --trim-quality 20 \
    --trim-mode both
```

## Recommended workflow fragments

### Look at statistics before deciding on filtering

```bash
FastQTools stat -i reads.fastq.gz -o stats.txt
FastQTools filter -i reads.fastq.gz -o clean.fastq.gz --min-quality 20 --min-length 50
```

### Let automation own log control

```bash
FastQTools --log-level error filter -i reads.fastq.gz -o clean.fastq.gz --min-quality 20
```

### Keep sidecar evidence

```bash
FastQTools stat -i reads.fastq.gz -o stats.txt --signature-report signatures.tsv
```

## Relationship between CLI and API

The CLI is the most direct engineering interface the project exposes. If you need to embed the capability into a C++ application instead of invoking it through the shell, continue to [`API Overview`](../api/overview) and [`Developer Architecture`](../dev/architecture).
