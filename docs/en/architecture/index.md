# Architecture

This page does not duplicate every internal module. It gives adopters a place to **build the mental model first, then decide where to drill down**. The question it should answer is: which concrete architecture choices actually explain FastQTools' performance and maintainability?

## One-sentence mental model

FastQTools uses a layered structure of **CLI / command layer / public library interface / concrete implementation**, carries FASTQ records through a **zero-copy batch-processing model**, organizes throughput around a **oneTBB parallel pipeline**, and uses **OpenSpec baseline + ADRs** to limit design drift over the long term.

That means it is neither a disposable script with commands but no real core, nor a pure SDK with no operational entry point. It brings user-facing entry points and the engineering core under the same design principles.

<DiagramFrame caption="Architecture overview: layered entry points feed a bounded source → processing → sink execution path.">
  <img src="../../assets/diagrams/architecture-overview.svg" alt="FastQTools architecture overview diagram showing the CLI, command layer, public API, implementation modules, and bounded processing pipeline." />
</DiagramFrame>

## Top-down view: who is responsible for what?

| Layer | How to think about it | Read more |
| --- | --- | --- |
| CLI layer | Handles argument parsing, command dispatch, and the logging boundary; this is the first layer terminal users see | [`CLI Reference`](../guide/cli-reference), [`Developer Architecture`](../dev/architecture) |
| Command layer | Translates tasks like `stat` and `filter` into concrete execution paths, with parameter validation and orchestration responsibilities | [`Getting Started`](../guide/getting-started), [`Developer Architecture`](../dev/architecture) |
| Public library layer | Exposes stable interfaces through `include/fqtools/`, so the CLI and external integrations rely on the same API surface | [`API Overview`](../api/overview) |
| Implementation layer | Actually performs I/O, processing, statistics, error handling, and configuration management; this is where performance and correctness land | [`Core Design`](../dev/design), [`IO Module`](../api/io), [`Processing Module`](../api/processing) |

If all you need to know is whether it is usable, the table above may be enough. If you care about why it behaves the way it does, the next section matters more.

## Left-to-right view: how data moves through the system

### 1. FASTQ is modeled as batch buffers plus record views, not as a bag of line strings

The core movement of data in FastQTools is that `FastqBatch` owns contiguous memory, while `FastqRecord` points into that batch memory through `std::string_view`. The benefit is not abstract elegance; it is practical:

- less string copying during parsing;
- batch processing combines more naturally with parallel pipelines;
- memory behavior on the hot path becomes easier to explain and constrain.

If you want to verify that this is not just documentation language, look directly at:

- [`Developer Architecture`](../dev/architecture)
- [`Core Design`](../dev/design)
- [`IO Module`](../api/io)
- [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)

### 2. Parallelism is not applied everywhere; it is organized around source / processing / sink

The project keeps reading and writing as sequential boundaries, and reserves true parallelism for the processing middle stage. That `source → processing → sink` structure means:

- I/O ordering is easier to guarantee;
- parallel capacity is spent mainly on filtering, trimming, statistics, and similar compute work;
- backpressure and in-flight resource limits are easier to describe and test.

That is also why the architecture material keeps emphasizing **bounded resources** and **deterministic output**: performance is not the only goal; understandable boundaries matter too.

### 3. Public API and implementation are separated so CLI use and embedded use share one core

A common adopter concern is this: if the CLI is pleasant to use, will I have to learn a different internal interface once I embed it into a C++ codebase? FastQTools tries to avoid that split:

- the CLI depends on the public API instead of binding directly to internals;
- `fq.h` acts as an aggregate external entry point;
- concrete modules stay subdivided into namespaces such as `io`, `processing`, `statistics`, `config`, and `error`.

If your adoption path includes “validate with the CLI first, then integrate as a library,” this separation is especially important.

## How the architecture turns into something trustworthy

The FastQTools documentation system does not just list module names. It also writes key correctness properties into the baseline:

- **memory safety**: FASTQ parsing and batch reuse should not lead to invalid access;
- **thread safety**: the parallel pipeline should not introduce data races;
- **repeatable, understandable output**: given the same input and configuration, results should remain consistently explainable;
- **bounded resources**: processing very large files should not let memory usage grow without bound.

The value of these commitments is that you can read them together with tests, sanitizers, and benchmarks, instead of treating architecture and verification as unrelated sets of materials.

## Recommended drill-down paths

### If you are evaluating adoption

1. Start with [`Why FastQTools`](../why-fastqtools/) to clarify the problem it solves;
2. Then read this page to build the layered and data-flow mental model;
3. Then go to [`Performance`](../performance/) to confirm whether those design choices actually map to evidence.

### If you are integrating it

1. Enter through [`API Overview`](../api/overview);
2. If you need to understand batch processing and record lifetimes, continue to [`IO Module`](../api/io);
3. If you need to understand filtering or trimming paths, continue to [`Processing Module`](../api/processing).

### If you are maintaining or contributing

1. Start with [`Developer Architecture`](../dev/architecture) and [`Core Design`](../dev/design);
2. Then cross-check the OpenSpec baseline / ADRs:
   - [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
   - [`RFC-0002: Toolchain Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)
   - [`RFC-0004: Memory Pool Optimization`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md)

## You do not need to read every detail here

The goal of this page is to help you judge:

- whether the performance story is grounded in concrete structure;
- whether the CLI, library interface, and internal implementation have clear boundaries;
- which deep document is actually worth your time next.

If the answer is yes, continue to:

- [`Workflows`](../workflows/): usage paths by role
- [`Performance`](../performance/): performance claims through evidence
- [`Reference`](../reference/): concrete manuals by task
