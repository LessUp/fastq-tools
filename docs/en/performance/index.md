# Performance

FastQTools puts performance docs in the narrative layer not to market numbers, but to help you judge **what those numbers mean, under what assumptions they hold, and when they are worth pursuing further.**

## Read the conclusion first, then decide whether to drill down

The current representative results in active maintenance come from **100K reads / 150 bp / AMD Ryzen 9 5900X / Release build**:

- FASTQ read throughput at about **1696 MB/s**
- FASTQ write throughput at about **1.76M reads/s**
- Combined filtering throughput at about **1.67M reads/s**
- Full statistics throughput at about **302 MB/s**

Those numbers first answer one question: **does FastQTools operate at the throughput tier you expect for FASTQ QC?** They are not a promise for every machine, every compression ratio, or every parameter combination, and they are not the whole answer to cross-tool benchmarking.

For more detail and context, see the [`Benchmark Report`](./benchmark-report).

## How this page should be used

### When you are evaluating adoption

Your question is usually not “does it have benchmarks?” but:

- do the benchmarks cover workloads similar to mine;
- are the numbers produced by a reproducible method rather than a lucky one-off run;
- does the architecture really explain the results;
- when the numbers change, does the project have a maintenance policy instead of leaving outdated screenshots behind.

That is why this page breaks performance evidence into four layers:

| Evidence layer | Purpose | Where to go |
| --- | --- | --- |
| Narrative layer | Decide whether the numbers are worth pursuing further | This page |
| Results layer | See representative metrics and summary findings | [`Benchmark Report`](./benchmark-report) |
| Method layer | See how benchmarks are run and how reports are produced | [`Benchmark Guide`](../dev/benchmark-guide) |
| Specification layer | See how long-term maintenance, thresholds, and SLA-like expectations are defined | [`RFC-0003`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md), [`RFC-0006`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md) |

## Why these results make sense architecturally

FastQTools does not present its performance story as isolated numbers. The claims are tied to the implementation path:

- the **zero-copy batch model** reduces string copying during parsing;
- the **oneTBB parallel pipeline** makes it easier to keep multicore processing busy;
- the **bounded-resource model** lets high throughput and controlled memory usage be discussed together;
- the **unified script entry points and benchmark maintenance policy** reduce the cost of explaining why something is fast locally, slower in CI, or outdated in documentation.

If you want to understand those reasons before looking at the benchmark numbers, start with [`Architecture`](../architecture/). If you arrived through the numbers first and only later want to inspect the design, that is equally valid.

## Where not to over-interpret the data

FastQTools deliberately calls these pages “representative results” to avoid several common mistakes:

1. **treating one hardware result as an SLA for every environment**;
2. **mixing the statistics path and the filtering path into the same metric**;
3. **ignoring how storage, compression, thread count, and data distribution affect throughput**;
4. **using the page as marketing collateral instead of a decision aid.**

If your real question is “can my environment reproduce these results,” go directly to:

- [`Benchmark Guide`](../dev/benchmark-guide)
- [`RFC-0006: Benchmark Maintenance and Release Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md)

## Typical reading paths

### Scenario A: I want to decide whether it is worth trying

[`Why FastQTools`](../why-fastqtools/) → [`Performance`](./) → [`Getting Started`](../guide/getting-started)

### Scenario B: I want to validate a throughput claim

[`Benchmark Report`](./benchmark-report) → [`Benchmark Guide`](../dev/benchmark-guide) → [`RFC-0003: Benchmark System`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md)

### Scenario C: I want to know whether the performance comes from a credible architecture

[`Architecture`](../architecture/) → [`Developer Architecture`](../dev/architecture) → [`Core Design`](../dev/design)

## Next step

- Want to return to the positioning question? [`Why FastQTools`](../why-fastqtools/)
- Want to understand the structural reasons? [`Architecture`](../architecture/)
- Want practical or API documentation next? [`Reference`](../reference/)
- Want project-internal and external follow-up material? [`Resources`](../resources/)
