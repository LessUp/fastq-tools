# FastQTools Context

FastQTools is a focused FASTQ quality-control toolkit. This context file records project-specific terms that help keep architecture reviews and future refactors aligned.

## Language

**FASTQ execution runtime**:
The internal module that owns batch-oriented FASTQ traversal and coordinates command-side work such as filtering and statistics.
_Avoid_: TBB plumbing, pipeline orchestration

## Relationships

- The **FASTQ execution runtime** is used by the `filter` and `stat` command paths.

## Example dialogue

> **Dev:** "Should the `stat` command keep its own batching code?"
> **Domain expert:** "No — batch traversal belongs in the **FASTQ execution runtime**, while `stat` should only provide its per-batch calculation."

## Flagged ambiguities

- "pipeline" currently refers both to per-read processing and whole-command execution; when discussing the shared traversal module, prefer **FASTQ execution runtime**.
