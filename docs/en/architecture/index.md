# Architecture

This page helps you build the system mental model for FastQTools: which layers own the entry points, which layers own throughput, which constraints keep the performance story explainable, and which resource boundaries keep it from being just a slogan about being “fast.”

## System layers

FastQTools uses a layered structure of CLI / command orchestration / public API / concrete implementation so terminal entry points, library interfaces, and internal code keep clear boundaries.

- **CLI and command layer** handles argument parsing, task orchestration, logging, and the error boundary so the terminal interface does not couple directly to low-level implementation detail.
- **Public API layer** exposes stable interfaces through `include/fqtools/`, letting the CLI, tests, and external C++ integrations depend on the same contract.
- **Implementation layer** carries I/O, processing, statistics, configuration, and error management, where performance, correctness, and bounded resources actually land.

The real value of this layering is not neat module names. It lets adopters answer whether they are using a throwaway command collection or an engineering core they can validate through the CLI and later embed into a program. For deeper drill-down, continue to [`API Overview`](../api/overview), [`Developer Architecture`](../dev/architecture), and [`Core Design`](../dev/design).

<DiagramFrame
  asset="architecture-overview"
  caption="Architecture overview: entry points, public API, and execution path are organized around the same whitepaper narrative."
/>

## Execution model

The system organizes data flow as `source → processing → sink`: reading and writing stay as clear boundaries, while the filtering, trimming, and statistics logic that benefits from parallelism lives in the middle processing stage.

The important point is not just that the project uses parallelism, but **how it does so and where the boundaries sit**:

1. **source** reads input files into bounded batches; `FastqBatch` owns contiguous buffers so records do not need to be copied into separate strings.
2. **processing** exposes record views through `std::string_view`, putting filtering, trimming, and statistics into the stage that can run in parallel.
3. **sink** writes results or summary outputs back out, keeping I/O ordering, error handling, and resource cleanup easy to reason about.

That means FastQTools is not a serial wrapper that reads a line, copies it, and rewrites it. It tries to minimize copying within the batch lifetime and then uses a staged pipeline so throughput and explainability live inside the same model. For the design basis behind that, continue to [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md) and [`RFC-0004: Memory Pool Optimization`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md).

## Key trade-offs

- Use a zero-copy batch model to reduce unnecessary copying, while strictly protecting `std::string_view` lifetimes.
- Use a parallel pipeline to gain throughput, while keeping resources bounded and outputs explainable.
- Use the public API to isolate the CLI from implementation detail, reducing later integration and maintenance cost.

Those trade-offs correspond to a few concrete correctness boundaries:

- **Lifetime correctness**: `std::string_view` must not outlive its owning batch, and a batch cannot be reused while processing still depends on its buffer.
- **Bounded resources**: in-flight batches, buffer reuse, and concurrency all need explicit limits so larger files do not imply uncontrolled memory growth.
- **Explainable output**: source and sink stabilize the input/output boundary, and middle-stage parallelism must not change the meaning of results under the same configuration.

If you want to tie these claims back to long-term maintenance constraints, revisit:

- [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
- [`RFC-0002: Toolchain Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)
- [`RFC-0004: Memory Pool Optimization`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md)

## Continue drilling down

If you want to connect this structure to benchmark evidence, go next to [`Performance`](../performance/). If you want a role-based path into action-oriented docs, continue to [`Workflows`](../workflows/). If you want to review implementation detail directly, move on to [`Developer Architecture`](../dev/architecture) and the [`IO Module`](../api/io).
