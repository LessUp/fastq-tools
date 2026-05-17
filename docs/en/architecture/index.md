# Architecture

Architecture is where the whitepaper stops being positioning language and becomes an inspectable systems model. The question here is not merely “what modules exist?” but “which boundaries keep FastQTools explainable when you move from adoption review into implementation review?”

## Layers that matter during review

FastQTools keeps the CLI, command orchestration, public API, and concrete implementations intentionally separated so that each audience has one stable surface to reason about.

- **CLI and command layer** owns argument parsing, task orchestration, logging, and the terminal-facing error boundary.
- **Public API layer** keeps `include/fqtools/` as the maintained integration surface for tests, the CLI, and downstream C++ callers.
- **Implementation layer** contains I/O, processing, statistics, configuration, and error handling where throughput and correctness are actually earned.

The layered model defined by <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge> is what lets FastQTools be reviewed first through the CLI and then through code-facing contracts without changing the mental model. That is also why the next narrative stop after this page is [`Algorithms`](../algorithms/) rather than an unstructured list of classes.

<DiagramFrame
  asset="architecture-overview"
  caption="Architecture overview: entry points, public API, and execution path are organized around the same whitepaper narrative."
/>

## Execution path and control surfaces

The maintained execution story is still `source → processing → sink`, but what matters architecturally is where control changes hands:

1. **Source** reads plain or gzip-compressed FASTQ into bounded batches.
2. **Processing** applies maintained predicates, mutators, and statistics work to record views instead of detached string copies.
3. **Sink** writes filtered records or summary output while preserving the final I/O and cleanup boundary.

That is why the algorithm layer matters: [`Algorithms`](../algorithms/) explains what each stage actually does, while this page explains why the stage boundaries exist in the first place. The architecture claim is not “parallelism exists,” but that the project makes parallelism reviewable by keeping ownership transitions explicit.

## Memory discipline is architectural, not incidental

The most important architectural trade-off is the decision to treat memory ownership as a first-class systems boundary:

- `FastqBatch` owns contiguous storage for multiple records;
- `std::string_view` keeps traversal cheap, but only while batch lifetime remains correct;
- object-pool reuse keeps allocation churn bounded, but only if in-flight batches are explicitly controlled.

That is the concern formalized in <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md">RFC-0004</ReferenceBadge>. It is also why the whitepaper can talk about performance and memory in the same breath without turning either topic into marketing shorthand.

## Verification ladder

Architecture alone does not prove the system works; it tells you where to verify next:

- go back to [`Whitepaper`](../whitepaper/) when you need the product and review framing;
- continue to [`Algorithms`](../algorithms/) to inspect filtering, statistics, execution, and memory behavior in maintained terms;
- continue to [`Performance`](../performance/) to see how architecture claims are reflected in benchmark evidence;
- use [`Research evolution notes`](../research/evolution-notes) when you want the historical reasons these boundaries were preserved.

## Continue drilling down

If you want to connect this structure directly to measurement, go next to [`Performance`](../performance/). If you want the maintained behavior behind the stages, read [`Algorithms`](../algorithms/). If you want implementation detail or code-level contracts, continue to [`Developer Architecture`](../dev/architecture), [`Core Design`](../dev/design), and the [`IO Module`](../api/io).
