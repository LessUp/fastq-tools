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

## What FastQTools is not trying to replace

FastQTools does not claim to subsume every role covered by FastQC, fastp, Cutadapt, or seqtk. The maintained product scope is narrower: routine FASTQ statistics plus filtering/trimming in a system whose execution model, benchmark evidence, and API boundary are deliberately documented together.

That means comparisons should focus on **boundary choice**:

- report-centric analysis versus reusable QC core;
- integrated “do everything in one command” preprocessing versus a smaller maintained capability set;
- compact utilities versus a documented architecture and public C++ surface.

## When the comparison becomes useful

Use this page when someone asks “why not just use FastQC, fastp, Cutadapt, or seqtk?” The right answer is rarely “because FastQTools has more features.” The stronger answer is that FastQTools chooses a different center of gravity: an auditable QC core with explicit execution and maintenance policy.

For the supporting sources behind these comparisons, continue to [`Bibliography`](./bibliography). For the internal system story that explains those boundaries, return to [`Whitepaper`](../whitepaper/) and [`Algorithms`](../algorithms/).
