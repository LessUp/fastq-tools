# Performance

The performance section is not just a place to display numbers. It helps you judge which architecture choices produced the results, under which assumptions they hold, and which boundaries can change the conclusion.

## Evidence summary

The representative snapshot currently cited by the whitepaper comes from a maintained standard scenario: **100K reads, 150 bp, AMD Ryzen 9 5900X, Release build**. Under that framing, the read path is about **1696 MB/s**, the write path about **1.76M reads/s**, combined filtering about **1.67M reads/s**, and full statistics about **302 MB/s**. The point of those numbers is not to prove the project is always that fast, but to show that FastQTools already operates at a throughput tier that matters for FASTQ QC, while keeping different paths, bottlenecks, and units separate.

The evidence is not a single table. This page explains why the numbers are credible, the [`Benchmark Report`](./benchmark-report) provides the representative snapshot, the [`Benchmark Guide`](../dev/benchmark-guide) explains how to reproduce it, and [`RFC-0003`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md) plus [`RFC-0006`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md) define the benchmark system and its maintenance boundary.

<DiagramFrame
  asset="execution-model"
  caption="Execution model: performance evidence should be read together with execution path, methodology, and maintenance boundaries."
/>

## How to read the benchmark material

Read it in the order **narrative layer → results layer → method layer → specification layer**. Start here to understand which question the numbers are trying to answer, move to the [`Benchmark Report`](./benchmark-report) for representative results, continue to the [`Benchmark Guide`](../dev/benchmark-guide) for sampling, build mode, and script entry points, and finally use RFC-0003 / RFC-0006 to decide whether the results are one-off observations, actively maintained snapshots, or release-facing constraints.

Do not stare only at the raw numbers. MB/s and reads/s are easy to flatten into a single conclusion even though reading, filtering, and statistics are different paths. It is also easy to miss input shape, thread settings, and release policy. For adopters, the better questions are usually whether the evidence covers a workload close to yours, whether the method is reproducible, whether the architecture explains the result, and whether the project defines regression and update policy.

## Risk boundaries

Published results are **representative samples**, not universal constants. At minimum, pay attention to these boundary conditions:

- **Compression ratio and codec cost**: gzip level and input compressibility directly affect CPU cost in read and write stages.
- **Storage I/O**: NVMe, network storage, container volumes, and shared filesystems can turn a benchmark into more of a disk test than a parsing test.
- **Thread count and concurrency parameters**: single-thread and multi-thread pipeline results are not directly comparable, and too many threads can add contention, scheduling noise, and NUMA effects.
- **Input distribution**: read length, quality distribution, predicate combinations, and pass rate all change the hotspots in the path.
- **Machine topology**: CPU microarchitecture, cache hierarchy, memory bandwidth, SMT, and container limits all affect the final curve.

So the numbers on this page are better for answering “is it worth evaluating further?” than for making capacity promises on your behalf. For migration or SLA decisions, go back to the project methodology and maintenance policy and reproduce the workload yourself.

## Recommended reading order

1. Read this page first to understand the evidence framework.
2. Then read the [`Benchmark Report`](./benchmark-report).
3. When you need reproduction details, continue to the [`Benchmark Guide`](../dev/benchmark-guide).
4. When you need the long-term maintenance framing, read [`RFC-0003`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md) and [`RFC-0006`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md).

## Return to the whitepaper storyline

If you have not yet built the system mental model, go back to [`Architecture`](../architecture/). If you are already ready to act, continue to [`Workflows`](../workflows/) or [`Reference`](../reference/).
