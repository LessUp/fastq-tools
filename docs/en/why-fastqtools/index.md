# Why FastQTools

FastQTools is not just another collection of FASTQ utilities. It turns the part of FASTQ quality-control work that is most repetitive, most likely to become a throughput bottleneck, and most in need of auditable results into a modern C++ toolkit that is explainable, embeddable, and maintainable.

If you are evaluating whether it is worth adopting, this page answers three questions first: **what problem it is trying to solve, how it differs from common alternatives, and where to go next to verify those claims.**

## The real question is not “can it process FASTQ,” but “can it process a lot of FASTQ reliably?”

In real sequencing QC pipelines, FASTQ handling is usually not conceptually hard, but it is constant and high-frequency:

- upstream systems keep producing compressed or uncompressed reads;
- downstream steps often need statistics, filtering, and trimming before alignment or assembly;
- once data volume grows, the bottleneck is often not the algorithm itself, but **I/O, string copying, thread utilization, and whether results remain reproducible**.

For many teams, the real pain is not “we need one more command,” but problems like these:

1. **A fragmented toolchain**: statistics, filtering, trimming, benchmarking, and release notes live across different scripts or different tools, so results become hard to explain consistently;
2. **Unpredictable performance**: the same FASTQ workload behaves very differently across machines and parameters, but the documentation does not tell you when the published numbers apply;
3. **High integration cost**: a CLI may be convenient, but once you need to embed it into a C++ program, an existing pipeline, or CI, stable interfaces are often missing;
4. **Weak maintenance evidence**: a project may call itself “high performance,” but never connect its architecture choices, benchmark method, and maintenance policy into one auditable story.

FastQTools is positioned to make that stretch of the pipeline feel more like a **technical whitepaper for adoption decisions**: not just commands, but commands, architecture, performance evidence, and maintenance boundaries presented in one reading frame.

## FastQTools stands out through engineering trade-offs, not feature count

### 1. Zero-copy FASTQ views are treated as a first-class design constraint

The core of FastQTools is not simply that it ships `stat` and `filter`. It is that **`FastqBatch` owns contiguous buffers while `FastqRecord` exposes record views through `std::string_view`**. That means the project is intentionally avoiding unnecessary copies during parsing, so performance discussion is grounded in a concrete data model instead of the vague idea that “C++ is fast.”

- Architecture summary: [`Architecture`](../architecture/)
- Deeper detail: [`Developer Architecture`](../dev/architecture)
- Related ADR: [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)

### 2. Parallel pipelines are designed to be explainable by default

The project explicitly uses oneTBB `parallel_pipeline` to organize a three-stage path for reading, processing, and writing, rather than hiding concurrency inside a black box. For adopters, that matters because you can clearly understand:

- which stage is sequential I/O;
- which stage is parallel filtering or transformation;
- why resource usage should remain bounded instead of scaling out of control with file size.

That kind of explanation directly affects whether you are comfortable putting it into a production QC pipeline or embedding it inside your own C++ service.

### 3. Performance claims are turned into a traceable evidence chain

FastQTools does not stop at publishing a benchmark table. It splits performance evidence into three layers:

- **Entry layer**: [`Performance`](../performance/) explains how to read the numbers;
- **Evidence layer**: [`Benchmark Report`](../performance/benchmark-report) provides representative results;
- **Method layer**: [`Benchmark Guide`](../dev/benchmark-guide) and the OpenSpec benchmark ADR explain how those results are generated and maintained.

The value of this structure is simple: you do not have to blindly accept that it is “fast.” You can follow the links and decide whether that speed is relevant to your workload.

### 4. It serves both CLI users and library integrators

In many projects, the command-line tool and the library surface become two separate worlds. FastQTools is closer to “one core, two entry points”:

- new users can start directly from [`Getting Started`](../guide/getting-started) and [`CLI Reference`](../guide/cli-reference);
- teams that need deeper integration can enter through [`API Overview`](../api/overview) and then drill into `io`, `processing`, and `statistics`;
- maintainers can continue down into [`Core Design`](../dev/design) and the related baseline / ADR material.

## Who it fits best, and what it is not

### Situations where it deserves serious evaluation

- You need dependable statistics, filtering, and trimming in the **pre-alignment QC** stage;
- You handle **large FASTQ collections or recurring batch jobs**, where throughput and resource boundaries matter more than flashy feature breadth;
- You want to **embed FASTQ processing into an existing C++ application**, not stay forever at the shell-script layer;
- You care whether the docs help new engineers understand why it is designed this way, where the numbers come from, and where the risk boundaries are.

### What not to misunderstand it as

FastQTools is not:

- a do-everything bioinformatics workflow orchestrator;
- a replacement for alignment, quantification, variant calling, or other downstream systems;
- a framework optimized primarily for plugin ecosystem breadth.

It is better understood as an **engineering core that does the FASTQ QC fundamentals well**: scoped tightly, backed by evidence, and practical to integrate.

## Suggested reading path before adoption

| The question you need answered | Read this first | Then continue with |
| --- | --- | --- |
| What problem is this project actually solving? | [`Why FastQTools`](./) | [`Knowledge Map`](../knowledge-map/) |
| Why might these design choices lead to performance gains? | [`Architecture`](../architecture/) | [`Developer Architecture`](../dev/architecture), [`Core Design`](../dev/design) |
| Are the benchmark numbers credible and relevant to my environment? | [`Performance`](../performance/) | [`Benchmark Report`](../performance/benchmark-report), [`Benchmark Guide`](../dev/benchmark-guide) |
| I want to run it first and validate the CLI experience | [`Workflows`](../workflows/) | [`Getting Started`](../guide/getting-started), [`CLI Reference`](../guide/cli-reference) |
| I need to integrate it into my own software | [`Reference`](../reference/) | [`API Overview`](../api/overview), [`Processing Module`](../api/processing) |

## Next step

- Want to inspect the core design trade-offs? Go to [`Architecture`](../architecture/)
- Want a role-based path through the docs? Go to [`Workflows`](../workflows/)
- Want to review the evidence directly? Go to [`Performance`](../performance/)
- Want the fastest route to specific manuals? Go to [`Reference`](../reference/)
