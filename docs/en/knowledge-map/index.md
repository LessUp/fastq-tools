# Knowledge Map

The FastQTools docs site is now organized into three layers: **the narrative layer explains judgment and navigation, the execution layer explains how to do the work, and the resource layer provides background and collaboration entry points**. The purpose of this map is to help you understand the site by problem domain instead of by folder name.

## How to read this map first

- **Narrative layer**: answers “why is this worth reading, where should I start, and how should I interpret the numbers?”
- **Execution layer**: answers “what command do I run, how do I use the API, and how do I validate development work?”
- **Resource layer**: provides supporting background, collaboration entry points, historical archives, and external references.

If you think of the whole site as a technical whitepaper:

1. the homepage creates interest and gives evidence entry points;
2. the narrative pages build the judgment framework;
3. the reference pages make the work actionable;
4. the resources pages connect site-local material to outside knowledge.

<DiagramFrame caption="Reading map: start with a narrative question, then move into execution details and supporting resources.">
  <img src="../../assets/diagrams/reading-map.svg" alt="FastQTools reading map showing narrative, execution, and resource layers with evaluation, running, integration, and maintenance entry points." />
</DiagramFrame>

## Read by problem domain

### 1. Evaluation: is this project a fit for my FASTQ QC scenario?

Start from these pages:

- [`Why FastQTools`](../why-fastqtools/) — first understand the positioning, differentiators, and scope boundaries;
- [`Architecture`](../architecture/) — then see the structural reasons behind the performance claims;
- [`Performance`](../performance/) — finally decide whether the benchmark evidence resembles your own scenario.

Supporting background (external):

- [FASTQ format (Wikipedia)](https://en.wikipedia.org/wiki/FASTQ_format) — useful for a quick refresh on the format itself;
- [NCBI SRA FASTQ guidance](https://www.ncbi.nlm.nih.gov/sra/docs/submitformats/) — useful for the broader context of FASTQ as an exchange format.

### 2. Running: I need to get commands working now

Start from these pages:

- [`Workflows`](../workflows/) — choose the reading path by goal;
- [`Getting Started`](../guide/getting-started) — get the first successful run quickly;
- [`CLI Reference`](../guide/cli-reference) — check concrete options and examples;
- [`Configuration`](../guide/configuration) / [`Deployment`](../guide/deployment) — handle environment setup and delivery.

Supporting background (external):

- [FastQC documentation](https://www.bioinformatics.babraham.ac.uk/projects/fastqc/) — useful for common QC-reporting context;
- [Cutadapt documentation](https://cutadapt.readthedocs.io/en/stable/) — useful for common adapter-trimming vocabulary.

### 3. Integration: I want to connect the capability to my own system

Read these first:

- [`Reference`](../reference/) — choose an entry point by intent;
- [`API Overview`](../api/overview) — establish the public surface;
- [`IO Module`](../api/io), [`Processing Module`](../api/processing), [`Statistics Module`](../api/statistics) — drill down by module;
- [`Architecture`](../architecture/) — revisit the boundaries and data flow.

Supporting background (external):

- [seqtk](https://github.com/lh3/seqtk) — a classic lightweight comparison point;
- [fastp](https://github.com/OpenGene/fastp) — an integrated QC-tool comparison point.

### 4. Maintenance: I need to confirm the specs, quality gates, and project history

Read these first:

- [`Developer Guide`](../dev/) — entry point for local build, testing, and quality tooling;
- [`Contributing`](../contributing) — collaboration expectations;
- [`Resources`](../resources/) — repository links, agent docs, and archive index in one place;
- OpenSpec baseline / ADRs:
  - [`Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
  - [`Toolchain Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)
  - [`Benchmark System`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md)

## Quick view of the site layers

| Layer | Main pages | What you get |
| --- | --- | --- |
| Narrative layer | [`Why FastQTools`](../why-fastqtools/), [`Architecture`](../architecture/), [`Workflows`](../workflows/), [`Performance`](../performance/) | Positioning, trade-offs, reading paths, evidence context |
| Execution layer | [`Reference`](../reference/), [`guide/`](../guide/), [`api/`](../api/), [`dev/`](../dev/) | Concrete commands, interfaces, and development flows |
| Resource layer | [`Resources`](../resources/), [`Contributing`](../contributing), [`archive/`](../archive/) | Collaboration entry points, historical material, extended links |

## If you only have 10 minutes

- Want to decide whether the project is worth more time? [`Why FastQTools`](../why-fastqtools/)
- Want the fastest route to practical usage? [`Workflows`](../workflows/)
- Want to browse the detailed material systematically? [`Reference`](../reference/)
- Want outside context and project-governance entry points? [`Resources`](../resources/)
