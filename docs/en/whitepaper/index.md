---
title: Whitepaper
description: Read FastQTools as a bounded FASTQ QC system with explicit architecture, algorithm, and evidence layers.
layout: doc
---

<SectionLandingGrid locale="en" section="whitepaper" />

# Whitepaper

## System overview

FastQTools is easiest to evaluate when you treat it as a deliberately narrow FASTQ quality-control engine. The maintained surface does not promise an all-purpose sequencing platform. It promises a disciplined `stat` and `filter` workflow, a reusable C++ integration layer, and the execution policies needed to make those surfaces explainable under review.

## Review questions

The whitepaper layer exists so an adopter can answer three questions before opening the source tree:

1. **What boundary is actually maintained?** FastQTools claims statistics, filtering, trimming, gzip-aware I/O, and bounded execution policy for FASTQ QC; it does not claim to replace arbitrary downstream bioinformatics stages.
2. **What systems model explains the performance posture?** Throughput claims are tied to zero-copy record views, bounded `FastqBatch` ownership, and oneTBB stage orchestration instead of vague “C++ is fast” language.
3. **Where does the proof trail live?** Architecture RFCs, benchmark policy, research references, and implementation reference pages remain visible so every headline claim can be audited later.

## What this layer is trying to prove

The point of this chapter is not to restate the README in longer form. It is to establish a review posture: read FastQTools as a bounded system with explicit contracts, explicit non-goals, and explicit follow-on chapters for architecture, algorithms, performance, and reference work.

The primary boundary documents behind this page are <ReferenceBadge kind="Spec" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/product/fastq-processing.md">Product Spec</ReferenceBadge>, <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge>, and <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md">RFC-0006</ReferenceBadge>.

## Boundary contract

FastQTools is strongest in the part of the sequencing workflow where teams need a repeatable QC and preprocessing core before heavier downstream stages begin. That means:

- it is **in scope** to discuss summary statistics, per-position metrics, supported filtering predicates, supported trimming mutators, gzip input/output handling, and the execution controls that keep throughput and memory bounded;
- it is **out of scope** to describe FastQTools as a workflow platform, classifier, mapper, or umbrella for unrelated sequencing stages;
- it is valid to compare the project with adjacent QC tools, but only when the comparison stays anchored to current capabilities instead of imagined roadmap features.

That distinction matters because the whitepaper is closer to a technical due-diligence packet than to a product-marketing landing page.

## Reading path

This page leads into the deeper whitepaper layers in the same order a technical review usually happens.

## Reading itinerary

Read the next layers in the same order you would review a system during technical evaluation:

1. [`Architecture`](../architecture/) — inspect the boundary ledger, control surfaces, and failure-containment decisions.
2. [`Algorithms`](../algorithms/) — inspect how filtering, statistics, execution policy, and memory discipline translate the architecture into maintained behavior.
3. [`Performance`](../performance/) — read the benchmark envelope together with methodology, caveats, and reproduction trail.
4. [`Research appendix`](../research/) — collect bibliography, adjacent-project context, and evolution notes once the main claims are already clear.

If you are still deciding whether FastQTools belongs in your environment, it also helps to read [`Why FastQTools`](../why-fastqtools/) before going deeper and [`Reference`](../reference/) once you need concrete commands, APIs, or implementation material.

## Reading order through the deep-content layers

The itinerary above is deliberate: narrative framing comes first, systems reasoning comes second, performance interpretation comes third, and operational lookup only appears once the reviewer already understands what is being operated.

## Core internal references

<ReferenceList locale="en" variant="site" />
