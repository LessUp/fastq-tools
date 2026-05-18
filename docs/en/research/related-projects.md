---
title: Related projects
description: Compare FastQTools with adjacent FASTQ and QC tools by boundary, not by hype.
layout: doc
---

# Related projects

This page is not a winner board. It is a boundary comparison for readers who need to place FastQTools in the broader FASTQ/QC tool landscape before making adoption claims.

## Comparison frame

| Project | Typical center of gravity | Why it matters here |
| --- | --- | --- |
| FastQC | Report-first quality assessment | Useful contrast when the question is “analysis report” rather than “embeddable QC core.” |
| fastp | Integrated preprocessing with filtering, trimming, and reports | Helpful when comparing a broad all-in-one executable with a narrower toolkit narrative. |
| Cutadapt | Adapter-focused trimming workflows | Important reference point for trimming semantics and workflow expectations. |
| seqtk | Small, portable FASTA/FASTQ utility toolkit | Useful contrast for readers who prefer compact command collections over layered docs and API surfaces. |
| FastQTools | Focused FASTQ QC core with CLI + C++ API | Best fit when the goal is explainable throughput, bounded memory behavior, and an embeddable interface. |

## Scope and evidence contrast

FastQTools does not claim to subsume every role covered by FastQC, fastp, Cutadapt, or seqtk. The maintained product scope is narrower: routine FASTQ statistics plus filtering/trimming in a system whose execution model, benchmark evidence, and API boundary are deliberately documented together.

That means comparisons should focus on **boundary choice**:

- report-centric analysis versus reusable QC core;
- integrated “do everything in one command” preprocessing versus a smaller maintained capability set;
- compact utilities versus a documented architecture and public C++ surface.

## Comparison axes

| Axis | What to compare | Why it matters |
| --- | --- | --- |
| Primary user action | Inspect a report, run preprocessing, trim adapters, script utility commands, or embed a QC core. | Tools with different first actions should not be ranked as if they solve the same problem. |
| Evidence style | Visual report, CLI result, benchmark envelope, RFC-backed policy, or source-level API contract. | Adoption review depends on how claims can be checked later. |
| Integration boundary | Standalone command, workflow component, library surface, or small utility collection. | A team embedding QC logic has different costs than a team opening a one-off report. |
| Maintenance posture | Feature breadth, documented constraints, benchmark governance, and compatibility expectations. | Long-term trust comes from knowing what the project will not claim. |

## What FastQTools is not trying to replace

Use this page when someone asks “why not just use FastQC, fastp, Cutadapt, or seqtk?” The strongest answer is rarely “because FastQTools has more features.” The stronger answer is that FastQTools chooses a different center of gravity: an auditable QC core with explicit execution and maintenance policy.

## When the comparison becomes useful

For the supporting sources behind these comparisons, continue to [`Bibliography`](./bibliography). For the internal system story that explains those boundaries, return to [`Whitepaper`](../whitepaper/) and [`Algorithms`](../algorithms/).
