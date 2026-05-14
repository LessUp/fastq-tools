# Resources

The resource hub gathers material that **does not belong in the main narrative, but still matters for both adoption and maintenance**: repository entry points, contribution paths, specification sources of truth, historical archives, and external references that provide background context.

## Internal entry points: from project facts to collaboration

### Project and collaboration

- [`Contributing`](../contributing) — for people preparing to submit documentation, test, or code changes;
- [`Changelog (bilingual; English-first)`](../release-notes/changelog) — for recent landed work;
- [GitHub repository](https://github.com/LessUp/fastq-tools) — the main source, issue, discussion, and release entry point;
- [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) — for concrete tasks and bug tracking.

### Specification and architecture sources of truth

You may not need these on the first read, but they matter when you need to verify what the project has actually committed to over time. To avoid confusion with older naming, this page consistently cites the OpenSpec architecture documents as `RFC-000X`:

- [OpenSpec baseline overview](https://github.com/LessUp/fastq-tools/tree/master/openspec/baseline)
- [RFC-0001: Core Architecture](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
- [RFC-0002: Toolchain Policy](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)
- [RFC-0003: Benchmark System](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md)
- [RFC-0006: Benchmark Maintenance Policy](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md)

### Automation and agent documentation

- [`Domain Model`](../agents/domain)
- [`Issue Tracker`](../agents/issue-tracker)
- [`Triage Labels`](../agents/triage-labels)

These pages matter more for maintenance and automation workflows than for first-time users, but they are valuable for anyone taking long-term ownership of the repository.

### Historical and migration material

- [`Archive Overview`](../archive/)
- [`Migration Notice`](../archive/migration-notice-2026-04-17)

When you encounter old docs, old scripts, or historical discussions, this group of links helps you decide what is still valid and what has already been superseded by OpenSpec.

## External background: understanding FASTQ and QC context

### FASTQ reference basics

- [FASTQ format (Wikipedia)](https://en.wikipedia.org/wiki/FASTQ_format) — for a quick refresh on the file structure and quality-score history;
- [NCBI SRA submit formats](https://www.ncbi.nlm.nih.gov/sra/docs/submitformats/) — for practical background on FASTQ as an exchange format.

### Quality-control and trimming context

- [FastQC](https://www.bioinformatics.babraham.ac.uk/projects/fastqc/) — for the common QC-reporting vocabulary used across the field;
- [Cutadapt documentation](https://cutadapt.readthedocs.io/en/stable/) — for adapter-trimming terminology and practice;
- [fastp](https://github.com/OpenGene/fastp) — for comparison with an integrated FASTQ preprocessing tool.

### Related tools and comparison reading

- [seqtk](https://github.com/lh3/seqtk) — a lightweight FASTA/FASTQ processing tool;
- [zlib-ng](https://github.com/zlib-ng/zlib-ng) / [libdeflate](https://github.com/ebiggers/libdeflate) — for the dependency choices behind the compression path;
- [oneTBB](https://github.com/oneapi-src/oneTBB) — for the concurrency library behind the parallel pipeline.

## Recommended ways to use this page

| What you need right now | Best first click |
| --- | --- |
| I want to contribute to the project | [`Contributing`](../contributing) |
| I want the official source of truth | OpenSpec baseline and related ADRs |
| I want external background knowledge | The FASTQ / FastQC / Cutadapt / fastp links |
| I want historical and migration context | [`Archive Overview`](../archive/) |

## How this page relates to the other navigation hubs

- If you want **site-internal reading paths**, start with [`Knowledge Map`](../knowledge-map/)
- If you want **concrete execution manuals**, start with [`Reference`](../reference/)
- If you want to understand **why the project exists**, return to [`Why FastQTools`](../why-fastqtools/)
