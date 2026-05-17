# Benchmark Report

This report provides the representative benchmark snapshot currently maintained to support the whitepaper’s performance narrative. It is not a replacement for the full methodology.

## Report purpose

First answer whether the result scale looks relevant to your workload, then decide whether you need to go deeper into scripts, method, or raw artifacts.

## Representative results

The following results come from the maintained representative scenario: 100K reads, 150 bp, AMD Ryzen 9 5900X, Release build.

| Workload | Representative result | Meaning |
| --- | --- | --- |
| FASTQ reading path | 1696 MB/s | Parsing and ingest throughput |
| FASTQ writing path | 1.76M reads/s | Output-stage throughput |
| Combined filtering path | 1.67M reads/s | Representative throughput under a multi-predicate QC workflow |
| Full statistics path | 302 MB/s | End-to-end statistics collection capability |

## How to read it together with the whitepaper

- Return to [`Performance`](./) for the evidence framing around these numbers.
- Read alongside [`Architecture`](../architecture/) to judge whether the results align with the execution model.
- Continue to the [`Benchmark Guide`](../dev/benchmark-guide) when you need method detail.

## Method reminder

These numbers are maintained representative snapshots, not guaranteed values for every environment. Storage, compression, thread count, predicate combinations, and input distribution can all change the final throughput.
