---
title: Reference
description: Move from the whitepaper narrative into exact commands, APIs, internals, and research lookup paths.
layout: doc
---

<SectionLandingGrid locale="en" section="reference" />

# Reference

Reference is the lookup surface for readers who already know the question they need to answer. The whitepaper establishes the system story; reference tells you where to verify exact behavior, exact interfaces, and exact maintenance material without forcing you through the whole narrative again.

## Use the deep-content layers intentionally

The content stack works best when each layer keeps one job:

- [`Whitepaper`](../whitepaper/) frames scope, constraints, and adoption logic.
- [`Architecture`](../architecture/) explains why the layers and boundaries exist.
- [`Algorithms`](../algorithms/) explains filtering, statistics, execution, and memory behavior in maintained terms.
- [`Performance`](../performance/) explains how to interpret throughput evidence.
- [`Research appendix`](../research/) adds bibliography, related-project comparison, and evolution notes once the core story is clear.

Reference begins after that point.

## Implementation lanes

Use this page when the task is no longer “understand the story” but “land on the exact maintained interface.” The reference layer therefore routes readers into command, API, build, and contributor surfaces instead of repeating narrative prose.

## Operator reference

- [`CLI Reference`](../guide/cli-reference)
- [`Getting Started`](../guide/getting-started)
- [`Configuration`](../guide/configuration)
- [`Deployment`](../guide/deployment)

## CLI and API lookup

- [`API Overview`](../api/overview)
- [`IO Module`](../api/io)
- [`Processing`](../api/processing)
- [`Statistics`](../api/statistics)

## Contributor surfaces

- [`Developer Guide`](../dev/)
- [`Developer Architecture`](../dev/architecture)
- [`Build`](../dev/build)
- [`Testing`](../dev/testing)
- [`Benchmark Guide`](../dev/benchmark-guide)

## Research and comparison entry points

- [`Research appendix`](../research/)
- [`Bibliography`](../research/bibliography)
- [`Related projects`](../research/related-projects)
- [`Evolution notes`](../research/evolution-notes)
- [`Archive Overview`](../archive/)

## Internal anchors from the whitepaper redesign

<ReferenceList locale="en" variant="site" />

## How to use this page

- Want to execute commands right now? Start from Operator reference.
- Want to understand public interfaces and library integration? Start from CLI and API lookup.
- Want to inspect internals, build, or test practice? Start from Contributor surfaces.
- Want project background, external reading, or comparison material? Move to the Research appendix.
