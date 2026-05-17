# Contributing

For FastQTools, a good contribution is not about packing in more features at once. It is about strengthening credibility at the right layer: make commands easier to explain, docs easier to follow, tests better at catching regressions, and benchmark conclusions easier to reproduce.

## Good directions for early contributions

- Fix broken reading paths, outdated commands, or unclear wording in the docs.
- Add more precise tests and regression coverage around existing behavior.
- Improve error messages, configuration explanations, and troubleshooting guidance.
- Keep benchmark evidence, research references, and release-facing material aligned.

## Establish shared context before contributing

1. Read the page or baseline directly related to what you want to change.
2. If the change affects public behavior, interfaces, or specifications, confirm whether the OpenSpec material also needs updates.
3. Keep the patch focused so reviewers can quickly understand why it changed, what changed, and how it was verified.

## Minimal local validation

```bash
./scripts/core/lint format
./scripts/core/test
```

If you are changing the docs site, also run the site tests and build from `docs/`. If you are changing the performance narrative, check that the related benchmark materials still align.

## Where to continue

- [`Developer Guide`](./dev/) — maintainer-facing architecture, build, and test material
- [`Resources`](./resources/) — repository, issues, OpenSpec, and external background links
- [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) — concrete tracked work
- [GitHub Discussions](https://github.com/LessUp/fastq-tools/discussions) — open-ended discussion
