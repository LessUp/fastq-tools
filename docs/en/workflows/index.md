# Workflows

The FastQTools workflows page is not a command cheat sheet. It translates “**what am I trying to accomplish right now?**” into an executable reading path. Most new users are not missing links; they are missing a fast way to decide which pages to read first so they can validate, integrate, or contribute efficiently.

## Choose your goal first, not the documentation directory

| Your current goal | Shortest path | Best for |
| --- | --- | --- |
| Decide whether it is worth trying further | [`Why FastQTools`](../why-fastqtools/) → [`Performance`](../performance/) → [`Getting Started`](../guide/getting-started) | Evaluators, technical leads |
| Run one FASTQ file end to end | [`Getting Started`](../guide/getting-started) → [`CLI Reference`](../guide/cli-reference) | New users, analysts |
| Put filtering / trimming into an existing QC pipeline | This page → [`Configuration`](../guide/configuration) → [`Deployment`](../guide/deployment) | Platform engineering, operations |
| Integrate the capability into a C++ application | This page → [`API Overview`](../api/overview) → [`Architecture`](../architecture/) | SDK and platform developers |
| Submit a patch or maintain the project | This page → [`Build Guide`](../dev/build) → [`Testing`](../dev/testing) → [`Contributing`](../contributing) | Contributors, maintainers |

## Path 1: complete an adoption evaluation in 30 minutes

If you are not yet sure whether this is worth real time, read in this order:

1. [`Why FastQTools`](../why-fastqtools/): confirm whether it is solving your problem at all;
2. [`Performance`](../performance/): decide whether the published numbers resemble your workload;
3. [`Getting Started`](../guide/getting-started): confirm whether installation and the first command are affordable in practice;
4. [`CLI Reference`](../guide/cli-reference): check whether the common options match your current QC habits.

**When is it reasonable to stop?**  
If, by this point, you already know the command model is sensible, the performance evidence is sufficient, and deployment cost looks manageable, you can move on to actual evaluation. If not, you can go deeper into architecture or benchmark material with explicit questions instead of reading the entire site blindly.

## Path 2: connect `stat` / `filter` to an existing QC pipeline

This is the most common real adoption path.

### You need to confirm more than command syntax

Beyond the commands themselves, you usually still need answers to questions like:

- is the input `.fastq` or `.fastq.gz`, and should the output stay compressed;
- how should thread count, batch size, log level, and environment limits be chosen;
- how should statistics output, filtered FASTQ, and sidecar files be passed to downstream steps;
- how do you keep local, CI, and container execution consistent.

### Suggested reading order

1. [`Getting Started`](../guide/getting-started) — first make the smallest command work;
2. [`CLI Reference`](../guide/cli-reference) — then fill in parameter-level detail;
3. [`Configuration`](../guide/configuration) — handle environment variables, config, and defaults;
4. [`Deployment`](../guide/deployment) — move the command into containers or production;
5. [`Performance`](../performance/) — if throughput is a key gate, return here for the evidence and assumptions.

## Path 3: move from “terminal tool” to “embeddable capability”

Many teams first prove value with the CLI, then decide whether to embed it into an internal C++ system. At that stage, you should not jump straight into source files. First confirm the interface boundaries.

Recommended order:

1. [`API Overview`](../api/overview) — see the public entry points and module layout;
2. [`Architecture`](../architecture/) — understand the relationship among the CLI, public API, and implementation layer;
3. [`IO Module`](../api/io) / [`Processing Module`](../api/processing) / [`Statistics Module`](../api/statistics) — drill down by integration focus;
4. [`Core Design`](../dev/design) — read this when you need the memory and concurrency trade-offs.

## Path 4: become a maintainer or submit a trustworthy patch

The FastQTools maintenance path emphasizes “**align to the baseline before touching code**.” So what you need first is not a specific `.cpp` file. You need to confirm the toolchain, verification entry points, and design boundaries.

Recommended order:

1. [`Build Guide`](../dev/build) — create a local environment through the repository-standard entry points;
2. [`Testing`](../dev/testing) — know which verification to run;
3. [`Quality Tools`](../dev/quality-tools) and [`Coding Standards`](../dev/coding-standards) — align with expected submission quality;
4. [`Developer Architecture`](../dev/architecture) and [`Core Design`](../dev/design) — understand which layer your change belongs to;
5. [`Contributing`](../contributing) — fill in the collaboration expectations;
6. If you need the source-of-truth specs, continue into the OpenSpec baseline:
   - [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
   - [`RFC-0002: Toolchain Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)

## Path 5: performance validation and regression investigation

Once the commands run, but you need to decide where performance is slow, whether a number is reasonable, or whether a regression is real, do not rely on a single benchmark table.

- Start with [`Performance`](../performance/) to understand how the public numbers should be read;
- then read the [`Benchmark Report`](../performance/benchmark-report) for representative results;
- finally read the [`Benchmark Guide`](../dev/benchmark-guide) and [`RFC-0003: Benchmark System`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md) to confirm method and maintenance policy.

## If you only want to know “where next?”

- **I want to try commands first**: [`Getting Started`](../guide/getting-started)
- **I want parameter definitions**: [`CLI Reference`](../guide/cli-reference)
- **I want to understand the design rationale**: [`Architecture`](../architecture/)
- **I want API and developer details**: [`Reference`](../reference/)
- **I want external background and collaboration links**: [`Resources`](../resources/)
