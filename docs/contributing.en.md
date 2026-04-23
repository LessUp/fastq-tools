# Contributing

FastQTools welcomes focused improvements: bug reports, docs fixes, tests, benchmark work, and narrow code changes that keep the project easier to trust and easier to use.

## Good first contributions

- Clarify setup steps or usage examples
- Improve error-case documentation and troubleshooting notes
- Add or tighten tests around existing behavior
- Reproduce or document benchmark workflows
- Review open issues and help confirm expected behavior

## Before you open a pull request

1. Check whether the change affects public behavior, docs, or examples, and keep those in sync.
2. Keep the scope small enough that reviewers can reason about it quickly.
3. If the change affects externally visible behavior, align it with the relevant OpenSpec baseline first.

## Local validation

```bash
# Format code when needed
./scripts/core/lint format

# Run the relevant checks
./scripts/core/test

# Build the docs site when touching public docs
mkdocs build --strict
```

## Where to start

- [Root CONTRIBUTING.md](https://github.com/LessUp/fastq-tools/blob/master/CONTRIBUTING.md) — full contribution process
- [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) — bugs and scoped work items
- [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions) — proposals and open-ended questions

## Docs contributions are valuable

A good documentation change is often the fastest way to improve the project for new users. README clarity, landing-page structure, command examples, and benchmark interpretation all directly affect whether people can adopt FastQTools confidently.
