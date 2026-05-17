# Developer Architecture

This page is for people modifying the implementation. Its concern is not a marketing diagram. It is the structural contract that has to remain stable during maintenance: how modules are layered, how data moves, and which breakpoints would directly harm performance or maintainability.

## System layers

FastQTools can be read as four layers:

1. **CLI layer**: parses arguments, organizes logging, and turns external input into explicit commands.
2. **Application command layer**: maps user intent such as `stat` and `filter` into internal configuration.
3. **Public API layer**: exposes stable interfaces through `include/fqtools/`.
4. **Implementation layer**: performs the actual read, processing, and statistics work under `src/io/`, `src/processing/`, `src/statistics/`, and related directories.

The most important maintenance rule is to keep the external boundary narrow while letting internals evolve without tangling the CLI, public headers, and implementation detail together.

## Execution model

The core execution model is `source → processing → sink`.

- `source`: reads input, decompresses, and parses FASTQ records.
- `processing`: applies predicates, trimmers, and statistics to batches.
- `sink`: writes outputs, aggregates statistics, and preserves required ordering.

On the parallel path, this is typically carried by `tbb::parallel_pipeline`. Maintainers need to keep the serial and concurrent boundaries clear, because many performance regressions come not from algorithm mistakes but from re-coupling work that used to be separated cleanly.

## Why `FastqBatch` matters

`FastqBatch` is a key entry point for understanding performance and memory behavior. It organizes records into reusable batch-sized memory units and minimizes unnecessary string copying.

When you modify I/O, filters, or statistics, ask yourself:

- Does this change break the batch-processing boundary?
- Does it introduce extra copying or lifetime risk?
- Does it force a stage that used to be parallel back into serial execution?

## Common maintainer decision points

- If you change user-visible behavior, check the CLI docs and API docs together.
- If you change the execution path, check whether the benchmark narrative still holds.
- If you change a module boundary, go back to the OpenSpec baseline and related ADRs to make sure you are not quietly changing an architectural commitment.
