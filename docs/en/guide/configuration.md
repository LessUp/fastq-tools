# Configuration

The configuration philosophy in FastQTools is simple: keep defaults, environment variables, and command-line parameters on one explicit override chain so users can always tell which setting wins.

## Configuration precedence

From low to high priority:

**defaults → config file / environment injection → environment variables → command-line arguments**

In practical use, that means:

- defaults provide a safe starting point;
- environment variables provide stable defaults for a given runtime environment;
- command-line arguments make the final decision for the current task.

## Command-line configuration

The most explicit and most auditable way to control behavior is to place key parameters directly in the command:

```bash
FastQTools stat -i reads.fastq.gz -o stats.txt -t 8
FastQTools filter -i reads.fastq.gz -o clean.fastq.gz --min-quality 20 --min-length 50
```

When a script is meant to live for a long time, keep the thresholds that directly affect results on the command line instead of hiding them in environment state.

## Environment variable configuration

When multiple tasks share the same runtime environment, environment variables work well as environment-level defaults:

```bash
export FASTQTOOLS_THREADS=8
export FASTQTOOLS_BATCH_SIZE=100000
FastQTools stat -i reads.fastq.gz -o stats.txt
```

If the same command also passes `--threads 16`, the command-line argument should override the environment variable. That matters especially in CI and containerized environments.

## Configuration boundaries for maintainers

Maintainers usually need to protect two properties:

1. **Explainable behavior**: a user should be able to read a command and infer the main runtime parameters.
2. **Maintainable defaults**: when new options are added, they should not break the override chain or create contradictory defaults across entry points.

If you are designing new options or tuning entry points, pair this page with [`Developer Architecture`](../dev/architecture) and [`Core Design`](../dev/design) instead of only adjusting CLI wording.

## Recommended practices

- Put result-affecting thresholds on the command line.
- Put environment-shaped defaults such as thread count and batch size into environment variables.
- Keep the same naming and precedence rules across containers, CI, and local scripts.
- In team settings, document which layer owns which choice.
