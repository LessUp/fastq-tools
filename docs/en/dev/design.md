# Core Design

The Core Design page focuses on why the current implementation style exists rather than listing every class one by one. It is best read after the architecture page, when you are ready to understand the data structures, configuration surface, and optimization trade-offs.

## Design principles

- **Measure before optimizing**: do not permanently add complexity without benchmark evidence or a clear reason.
- **Zero-copy discipline**: design around FASTQ record views and batch boundaries to avoid unnecessary copies on the hot path.
- **Engineering consistency**: build, tests, CLI, docs, and baseline specifications should reinforce each other instead of drifting apart.

## Data and I/O design

The project is organized around FASTQ record views, batch buffers, and separated reader/writer responsibilities. The reader turns input into consumable batches. The writer emits processed results consistently. The middle stages should touch only the information they need instead of inventing a second parallel data model.

## Pipeline design

`stat` and `filter` share the same engineering pattern:

- organize input into units that can be processed in batches;
- apply filtering, trimming, or statistics across clear stage boundaries;
- emit outputs together with traceable evidence.

That is why many “small convenience changes” can be harmful. Examples include pushing business exceptions into CLI parsing, placing debug logic in the hot path, or making a single module interpret configuration and process data at the same time.

## When you should stop and update the spec

If a change affects the public API, configuration format, behavior boundary, or benchmark narrative, it is no longer only an implementation detail. At that point, return to the OpenSpec baseline or changes process instead of hiding the change inside code.
