# Performance

Performance evidence only matters if it stays attached to system boundaries. This page therefore treats benchmark numbers as a maintained argument: which workloads are represented, which assumptions are fixed, and which parts of the conclusion must stay conditional.

## Evidence summary

The representative snapshot cited in the maintained docs still comes from the standard scenario documented in the baseline: **100K reads, 150 bp, AMD Ryzen 9 5900X, Release build**. Under that framing, the read path is about **1696 MB/s**, the write path about **1.76M reads/s**, combined filtering about **1.67M reads/s**, and full statistics about **302 MB/s**.

The point of repeating those numbers is not to promise that every dataset or storage path will behave the same. The point is to show that FastQTools already lands in a throughput class that matters for FASTQ QC **and** that the project publishes enough method context to keep those numbers reviewable.

<DiagramFrame
  asset="execution-model"
  caption="Execution model: performance evidence should be read together with execution path, methodology, and maintenance boundaries."
/>

## The evidence ladder

Treat the performance material as a ladder, not as a single benchmark table:

1. **Narrative layer** — this page explains what question the numbers are meant to answer.
2. **Results layer** — [`Benchmark Report`](./benchmark-report) shows the representative snapshot.
3. **Method layer** — [`Benchmark Guide`](../dev/benchmark-guide) explains how results are reproduced.
4. **Policy layer** — <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md">RFC-0003</ReferenceBadge> and <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md">RFC-0006</ReferenceBadge> define collection, storage, thresholds, and release-facing interpretation.

If you skip a layer, you usually end up overstating the conclusion. Architecture still matters here, which is why it helps to read this page alongside [`Architecture`](../architecture/) and [`Algorithms`](../algorithms/).

## What can change the conclusion

Published results are **representative samples**, not universal constants. The most important moving parts are:

- **Compression ratio and codec cost**: gzip level and input compressibility directly affect CPU cost in read and write stages.
- **Storage I/O**: NVMe, network storage, container volumes, and shared filesystems can turn a benchmark into more of a disk test than a parsing test.
- **Thread count and concurrency parameters**: single-thread and multi-thread pipeline results are not directly comparable, and too many threads can add contention, scheduling noise, and NUMA effects.
- **Input distribution**: read length, quality distribution, predicate combinations, and pass rate all change the hotspots in the path.
- **Machine topology**: CPU microarchitecture, cache hierarchy, memory bandwidth, SMT, and container limits all affect the final curve.

So the right question is usually not “is 1696 MB/s the truth?” It is “does the project publish a result, method, and policy that make further evaluation worthwhile?” For migration or SLA work, reproduce the workload yourself and use the policy documents before repeating any number.

## Cross-checks beyond the benchmark report

Once the raw metrics are clear, use the research layer to avoid reading them in isolation:

- [`Research bibliography`](../research/bibliography) collects the formal sources behind format, architecture, and benchmark language.
- [`Related projects`](../research/related-projects) helps you compare FastQTools with FastQC, fastp, Cutadapt, and seqtk without flattening them into a winner table.
- [`Evolution notes`](../research/evolution-notes) explains why benchmark policy and memory policy were elevated into maintained architecture decisions.

## Return to the whitepaper storyline

If you have not yet built the system model, go back to [`Whitepaper`](../whitepaper/) and [`Architecture`](../architecture/). If you want the maintained behavior behind the measured paths, continue to [`Algorithms`](../algorithms/). If you are ready to act on exact commands or APIs, move to [`Reference`](../reference/).
