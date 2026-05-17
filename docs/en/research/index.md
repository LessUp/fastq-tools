---
title: Research appendix
description: Collect bibliography, adjacent project context, and design evolution for the English whitepaper.
layout: doc
---

<SectionLandingGrid locale="en" section="research" />

# Research appendix

The research appendix is the layer you open after the main whitepaper claims are already clear. It is not the place to learn the first command. It is the place to answer “what supports these claims?”, “how does this compare to nearby tools?”, and “why was the current boundary chosen?”

## What this layer contains

- [`Bibliography`](./bibliography) collects the papers, specifications, RFCs, library docs, and project pages that support the current narrative.
- [`Related projects`](./related-projects) compares FastQTools with FastQC, fastp, Cutadapt, and seqtk in terms of scope, interface boundary, and evidence style.
- [`Evolution notes`](./evolution-notes) explains how architectural and benchmark policy decisions accumulated into the current system narrative.

## References

Use [`Bibliography`](./bibliography) when you need the source trail behind format claims, benchmark language, or adjacent-project comparisons.

## How to use the research layer well

Use the appendix when one of these situations appears:

- you need to cite an external source or internal RFC instead of paraphrasing from memory;
- you need to explain how FastQTools differs from adjacent QC or preprocessing tools without turning the comparison into a feature race;
- you need historical context for why benchmark policy, memory policy, and documentation boundaries were made explicit.

## Relationship to the rest of the docs

If you still need the main product and architecture story, go back to [`Whitepaper`](../whitepaper/) and [`Architecture`](../architecture/). If you already know the question and need exact commands or APIs, go to [`Reference`](../reference/). The research layer is deliberately the place in between: technical context without pretending to be the operational manual.

## Design evolution

Use [`Evolution notes`](./evolution-notes) when the question is not “how do I run this?” but “why was this boundary chosen and maintained?”
