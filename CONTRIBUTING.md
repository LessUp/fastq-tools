# Contributing to FastQTools

Thanks for helping improve FastQTools.

## Before you start

- Review the relevant baseline spec in [`openspec/baseline/`](openspec/baseline/).
- If your change affects behavior, public APIs, schemas/file formats, architecture, toolchain policy, or compatibility, create or update a proposal under [`openspec/changes/`](openspec/changes/) before implementation.
- Docs-only, test-only, and internal cleanup changes that do not alter baseline meaning can go straight to implementation, but the commit or push should cite the relevant baseline section or state `no baseline delta`.
- If you use repository agents, also follow [`AGENTS.md`](AGENTS.md).

## Preflight

Run this lightweight check set before you start a task:

```bash
git status --short --branch
```

- Make sure the current tree is in a state you can safely continue from.
- Use `git fetch --prune origin`, `git worktree list`, or `gh` only when the task actually needs remote inspection or extra isolation.

## Branch workflow

For this solo-maintained repository, direct push is the default. Use an extra branch or worktree only when it genuinely helps isolate risky changes:

```bash
git checkout -b fix/<slug>   # optional
# or: git worktree add ../fastq-tools-<slug> -b fix/<slug> master
```

- Use `fix/<slug>`, `docs/<slug>`, or `chore/<slug>` depending on the change.
- Small, focused changes are preferred whether you push directly or use a short-lived branch.
- Run only the checks relevant to your change. For docs-only changes, `git diff --check` is the minimum.
- Push directly once the relevant checks pass and the baseline/proposal state is correct.

## Local checks

Use the maintained `scripts/core/` entry points instead of legacy script paths:

```bash
./scripts/core/install-deps
./scripts/core/build --dev
./scripts/core/lint check
./scripts/core/test
```

Keep changes small and focused, add or update tests for behavior changes, and update user-facing docs when needed.

## Commit messages

Prefer Conventional Commits such as `feat: ...`, `fix: ...`, `docs: ...`, or `test: ...`.

## More guidance

- OpenSpec overview: [`openspec/README.md`](openspec/README.md)
- Git workflow details: [`docs/dev/git-guidelines.md`](docs/dev/git-guidelines.md)
- Core script reference: [`scripts/core/README.md`](scripts/core/README.md)
- Developer documentation: [`docs/dev/index.md`](docs/dev/index.md)
- Toolchain policy: [`openspec/baseline/architecture/0002-toolchain-policy.md`](openspec/baseline/architecture/0002-toolchain-policy.md)

## Security

- See `SECURITY.md` for reporting vulnerabilities.

## License

- By contributing, you agree that your contributions will be licensed under the project license (MIT unless otherwise specified).
