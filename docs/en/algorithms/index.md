---
title: Algorithms
description: Inspect the maintained filtering, statistics, execution, and memory behavior behind FastQTools.
layout: doc
---

# Algorithms

The algorithm layer is where the whitepaper becomes operationally precise. It does not introduce new product surface. Instead, it explains how the maintained `stat` and `filter` capabilities are structured today so that reviewers can reason about correctness, performance, and resource boundaries without immediately diving into source code.

## Filtering pipeline

The filtering path is deliberately narrow and composable. The current baseline supports:

- **predicates** such as minimum average quality, minimum length, maximum length, and maximum `N` ratio;
- **mutators** such as quality trimming, adapter trimming, polyG trimming, and bounded polyX trimming;
- **single-pass execution** so filtering and trimming can be applied without rebuilding the input stream multiple times.

That means the filtering story is not “every preprocessing transform we could imagine.” It is a focused path in which record validity, sequence/quality alignment, and bounded throughput remain more important than feature sprawl. For CLI behavior, continue to [`CLI Reference`](../guide/cli-reference); for the system reason these stages exist, go back to [`Architecture`](../architecture/).

## Statistics path

The statistics path exists to make QC decisions explainable. The `stat` command is responsible for summary metrics and per-position measurements, not just a terminal success/failure code. In maintained terms, this includes read counts, length distribution, base composition, GC content, and quality-oriented metrics such as Q20/Q30-style summaries.

The product baseline also allows optional lightweight signatures alongside the default text report. The important constraint is that the sidecar remains additive: it may extend analysis, but it does not replace the primary report contract. That distinction matters when the docs compare FastQTools with report-centric tools in the research appendix.

## Execution model

FastQTools processes data through the same staged model described in the architecture layer: `source → processing → sink`. The execution commitment behind that model is straightforward:

1. the reader owns ingestion and batch formation;
2. the processing stage owns filtering, trimming, and statistics work that benefits from concurrency;
3. the sink owns ordered output, summary emission, and the final error boundary.

The maintained concurrency story is based on oneTBB `parallel_pipeline`, but the main point for readers is not the library brand. It is that execution remains staged, reviewable, and bounded. You can therefore read performance evidence and implementation behavior through the same model instead of learning separate stories for “fast mode” and “correct mode.”

## Memory discipline

Memory policy is part of the algorithm layer because the throughput story depends on it. `FastqBatch` keeps contiguous storage for record data, and record fields are exposed as `std::string_view` while the owning batch remains alive. That reduces needless copying, but it also creates a hard correctness rule: views must never outlive the batch that owns their storage.

The object-pool strategy described in <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md">RFC-0004</ReferenceBadge> exists to control allocation churn and to keep in-flight memory bounded. This is why memory is not a postscript to performance; it is one of the maintained execution constraints.

## Correctness invariants

The algorithm layer should be judged by invariants, not only by throughput:

| Invariant | Why it matters |
| --- | --- |
| FASTQ records remain four-line logical units. | Parsing speed is irrelevant if record structure becomes ambiguous. |
| Sequence and quality lengths stay aligned after mutation. | Filtering and trimming must not produce invalid FASTQ output. |
| Predicate decisions are deterministic for the same input and configuration. | Reproducible QC is more important than opportunistic parallel ordering. |
| Batch views never escape their owning storage. | Zero-copy is only safe when lifetime discipline is explicit. |
| Signature sidecars remain additive. | Optional QC summaries must not replace the default `stat` report contract. |

## Where to verify next

- Read [`Performance`](../performance/) to see how these execution choices surface in benchmark evidence.
- Read [`API Processing`](../api/processing) and [`API Statistics`](../api/statistics) for interface-facing detail.
- Read [`Research appendix`](../research/) for bibliography, related-project context, and evolution notes.
