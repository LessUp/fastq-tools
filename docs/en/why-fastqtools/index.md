# Why FastQTools

FastQTools is aimed at the part of FASTQ quality control that is most common, most sensitive to stable throughput, and most in need of auditable output. It is not trying to rebuild the entire bioinformatics pipeline as a platform.

## What problem it is trying to solve

Many teams do not lack a command that can “process FASTQ.” What they actually lack is an engineering toolkit that explains adoption logic, performance assumptions, maintenance boundaries, and integration paths in one place.

The deciding questions are usually engineering questions like these:

- **Can throughput be explained?** As FASTQ volume grows, bottlenecks usually come from I/O, string copying, thread utilization, and backpressure, not just the algorithm itself.
- **Can results be audited?** Once filtering, trimming, and statistics enter production paths, resource boundaries, output consistency, and verification paths have to be clear.
- **Is integration cost controllable?** Many teams validate with the CLI first, then embed the capability into a C++ program or an existing QC workflow. If the interfaces split, adoption cost rises quickly.

FastQTools brings those questions into one whitepaper narrative instead of scattering commands, benchmark numbers, and architectural trade-offs across disconnected pages. If you want the maintained reading spine first, start from the [`Whitepaper`](../whitepaper/) and keep the [`Reference`](../reference/) surface nearby for exact commands and API follow-up.

## Suitable scenarios

- You need stable statistics, filtering, and trimming in the pre-alignment stage.
- You need to process large FASTQ collections in batches and care about throughput and resource boundaries.
- You want to validate through the CLI first and then embed the capability into a C++ application or an existing QC flow.

## How it differs from adjacent solutions

The difference is not “a longer feature list.” It is that FastQTools writes **zero-copy batch processing, the parallel execution model, benchmark evidence, and long-term maintenance constraints** into one narrative chain so adopters can judge whether the claims are credible.

If you are evaluating why it may be worth adopting, focus on three follow-up pages:

1. [`Architecture`](../architecture/) explains how `FastqBatch`, `std::string_view`, and the `source → processing → sink` path shape throughput and resource boundaries.
2. [`Performance`](../performance/) does not just present results; it explains how to read the benchmarks and where the risk boundaries sit.
3. [`Reference`](../reference/) and [`API Overview`](../api/overview) show that this is not only a one-off shell command layer, but something you can keep drilling into as library and developer material.

That makes FastQTools closer to an **auditable engineering core** than to a generic command wrapper.

## Who it is not for

FastQTools may not be the best fit if your need is closer to one of these cases:

- you need a **full-stack platform** that also covers alignment, quantification, variant calling, and later pipeline stages;
- you care more about a script-plugin ecosystem than about a core implementation with explicit resource boundaries and a C++ API;
- you just want a miscellaneous FASTQ helper and do not care about performance evidence, maintenance limits, or embeddability.

In other words, it should not be misunderstood as an everything framework. It is more focused on doing the FASTQ QC fundamentals well, making them explainable, and keeping them integrable over the long term.

## How to verify these claims next

- Read [`Architecture`](../architecture/) for the system layers and execution model.
- Read [`Performance`](../performance/) for how the evidence should be interpreted.
- Read [`Workflows`](../workflows/) to decide which execution path fits you next.
- Read [`Developer Architecture`](../dev/architecture) and [`Core Design`](../dev/design) to inspect finer implementation trade-offs.
