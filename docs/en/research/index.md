---
title: Research appendix
description: Add comparison reading, references, and design-evolution context to the English whitepaper.
layout: doc
---

<SectionLandingGrid locale="en" section="research" />

# Research appendix

The research appendix is neither a product manual nor a contribution checklist. It is closer to the appendix of the whitepaper: a place for comparative reading, source material, historical context, and future-looking questions so readers can place FastQTools back into the broader FASTQ/QC landscape.

## Related projects

Compare FastQTools with adjacent FASTQ/QC tools in terms of positioning, interface boundaries, and evidence style.

The goal is not to build a winner board. It is to see how different tools choose their boundaries: some emphasize integrated preprocessing, some emphasize small portable commands, while FastQTools emphasizes an engineered pipeline, explainable performance evidence, and clear interface layers.

## References

List papers, standards, official docs, and key repositories.

Start with three source groups: FASTQ format and quality-score background, the official docs of common QC and trimming tools, and FastQTools' own dependencies or comparison targets such as oneTBB, FastQC, fastp, Cutadapt, and seqtk. Together these sources make the terms, boundaries, and comparisons in the whitepaper credible.

## Design evolution

Explain why the current architecture exists and how it might evolve.

The current design works because the project intentionally separates the narrative layer, the reference layer, and the maintenance layer: first establish positioning, then provide operations, then collect maintenance knowledge. If the project expands into more commands, language bindings, or finer-grained benchmarks, it should keep evolving along that layered model rather than collapsing everything back into a single landing page.
