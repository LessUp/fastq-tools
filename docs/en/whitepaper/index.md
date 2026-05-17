---
title: Whitepaper
description: Read FastQTools as a bounded FASTQ QC system with explicit architecture, algorithm, and evidence layers.
layout: doc
---

<SectionLandingGrid locale="en" section="whitepaper" />

# Whitepaper

## System overview

FastQTools makes the most sense when you read it as an engineering core for routine FASTQ quality control, not as a generic “sequence swiss-army knife.” The maintained product surface is intentionally narrow: a `stat` command for analysis, a `filter` command for filtering and trimming, and a reusable C++ interface for teams that need to embed the same primitives in larger systems.

## What this layer is trying to prove

The whitepaper layer exists so an adopter can answer three review questions without reading source code first:

1. **Is the product boundary honest?** The maintained baseline only promises focused FASTQ QC work: statistics, filtering, trimming, gzip-aware I/O, and bounded execution policy through the existing `stat` / `filter` workflows.
2. **Is the performance story explainable?** Throughput claims are tied to zero-copy batch handling, `std::string_view` record views, oneTBB pipeline stages, and bounded memory reuse rather than hand-wavy “fast because C++” language.
3. **Can the claims be audited later?** Architecture RFCs, benchmark policy, and the developer-facing docs all stay available so the adoption narrative can be checked against maintained source material.

The primary boundary documents behind this page are <ReferenceBadge kind="Spec" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/product/fastq-processing.md">Product Spec</ReferenceBadge>, <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge>, and <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md">RFC-0006</ReferenceBadge>.

## Scope, constraints, and non-goals

FastQTools is strongest in the part of the sequencing workflow where teams need a repeatable QC and preprocessing core before heavier downstream stages begin. That means:

- it is **in scope** to discuss summary statistics, per-position metrics, supported filtering predicates, supported trimming mutators, gzip input/output handling, and the execution controls that keep throughput and memory bounded;
- it is **not in scope** to describe FastQTools as a workflow platform, classifier, mapper, or a maintained umbrella for unrelated sequencing stages;
- it is valid to compare the project with adjacent QC tools, but only if the comparison stays anchored to present-day capabilities instead of imagined roadmap features.

That distinction matters because the whitepaper is deliberately closer to a technical due-diligence packet than to product marketing copy.

## Reading path

This page leads into the deeper whitepaper layers in the same order a technical review usually happens.

## Reading order through the deep-content layers

Read the next layers in the same order you would review a system during technical evaluation:

1. [`Architecture`](../architecture/) — understand the layer boundaries, the `source → processing → sink` execution shape, and the architectural role of bounded memory reuse.
2. [`Algorithms`](../algorithms/) — inspect how filtering, statistics, execution policy, and memory discipline translate the architecture into maintained behavior.
3. [`Performance`](../performance/) — read the benchmark posture together with its methodology and risk boundaries.
4. [`Research appendix`](../research/) — collect bibliography, adjacent-project context, and design evolution notes after the main claims are already clear.

If you are still deciding whether FastQTools belongs in your environment, it also helps to read [`Why FastQTools`](../why-fastqtools/) before going deeper and [`Reference`](../reference/) once you need concrete commands, APIs, or implementation material.

## Core internal references

<ReferenceList locale="en" variant="site" />
