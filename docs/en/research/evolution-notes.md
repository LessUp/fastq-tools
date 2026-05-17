---
title: Evolution notes
description: Trace how architecture, memory, and benchmark policy decisions shaped the current FastQTools narrative.
layout: doc
---

# Evolution notes

FastQTools did not arrive at its current documentation and architecture shape by accident. The maintained system story is the result of several decisions that turned implementation concerns into explicit public reasoning material.

## Core architecture became the stable spine

<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge> established the layered architecture, the zero-copy batch model, and the `source → processing → sink` execution path. That decision matters beyond code organization: it made it possible for the CLI, API, and benchmark story to reuse the same system frame.

## Memory policy moved into first-class design language

<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md">RFC-0004</ReferenceBadge> made memory reuse and in-flight bounding explicit instead of treating allocation behavior as an implementation footnote. Once record views depend on batch lifetime, memory policy becomes part of correctness, not only part of optimization.

## Benchmark publication became governed, not anecdotal

<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md">RFC-0003</ReferenceBadge> defined how benchmark data is collected and stored. <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md">RFC-0006</ReferenceBadge> then separated release-facing SLA language from informational GitHub Pages benchmark publication. Together they changed performance from “interesting numbers” into maintained evidence.

## Documentation now mirrors that engineering split

The current docs separate narrative, operational, and research responsibilities:

- [`Whitepaper`](../whitepaper/) carries product scope and review framing.
- [`Architecture`](../architecture/) and [`Algorithms`](../algorithms/) carry system and execution reasoning.
- [`Performance`](../performance/) carries evidence interpretation.
- [`Reference`](../reference/) carries exact lookup material.
- [`Research appendix`](../research/) carries bibliography, comparison context, and historical notes.

That split is useful because it keeps the main product story short while preserving the deeper material reviewers actually need.

## What should remain stable

Even if the project grows, the key discipline should remain the same: new capabilities should not collapse narrative, implementation, evidence, and reference material back into one undifferentiated landing page. The current structure works because each layer answers a different question with a different level of proof.
