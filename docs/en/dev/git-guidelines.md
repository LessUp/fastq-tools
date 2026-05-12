# Git Workflow (Maintenance Phase)

## 1. Preflight

Before starting a new task, run at least:

```bash
git status --short --branch
```

- `git status --short --branch` confirms whether the current directory is safe to edit.
- `git fetch --prune origin`, `git worktree list`, `git branch -vv`, and `gh` checks are optional and only needed when the task actually requires them.

## 2. Branches and worktrees

Maintenance-phase work in this solo repository can be committed and pushed directly. Use an extra branch or worktree only when you need isolation for risky or parallel work:

```bash
git checkout -b fix/<slug>   # optional
# or: git worktree add ../fastq-tools-<slug> -b fix/<slug> master
```

- Recommended prefixes: `fix/<slug>`, `docs/<slug>`, `chore/<slug>`.
- One worktree should map to one task.
- Direct work on `master` is allowed if the change is small and well-controlled.
- If multiple AI tools participate, keep only one primary editor for the active changes.

## 3. Development and lightweight validation

- Keep commits small and focused.
- Docs/rules changes should run at least `git diff --check`.
- Code changes should use the existing entry points: `./scripts/core/build`, `./scripts/core/test`, and `./scripts/core/lint`.
- If a change affects behavior, API, schema, architecture, toolchain, or compatibility, update `openspec/changes/` before implementation.

## 4. Push and optional review

Useful commands:

```bash
git push origin master
# or push the current branch
git push origin fix/<slug>
```

- Before pushing, record baseline references, validation results, and whether the change is `no baseline delta`.
- If the change is risky, add an optional human or secondary-AI diff review.
- If you find proposal/baseline drift, update the docs first, then continue.

## 5. Cleaning up local worktrees and branches

After the change is safely committed or pushed and the worktree is no longer needed:

```bash
git worktree remove ../fastq-tools-<slug>
git branch -d fix/<slug>
```

If remote branches were deleted, prune regularly:

```bash
git fetch --prune origin
git branch -vv
```

## Commit Format

```
<type>(<scope>): <subject>
```

## Type Categories

| Type | Description |
|------|-------------|
| `feat` | New feature |
| `fix` | Bug fix |
| `docs` | Documentation changes |
| `style` | Code style (no functional impact) |
| `refactor` | Code refactoring |
| `perf` | Performance optimization |
| `test` | Add or modify tests |
| `build` | Build system or dependency changes |
| `ci` | CI/CD configuration changes |
| `chore` | Other maintenance changes |

## Scope (Optional)

- `core`: Core functionality
- `cli`: Command line interface
- `parser`: Parser
- `devops`: Operations related
- `docs`: Documentation related

## Subject

- Use imperative mood, present tense
- Don't capitalize first letter
- No period at the end

## Examples

```
feat(parser): add BAM file input support
fix(core): correct GC content calculation
docs(readme): update installation instructions
refactor(pipeline): simplify processing loop
perf(io): optimize buffer allocation
ci(github): add automated linting
```

## Commit Message Template

```bash
# Repository template file: .gitmessage.txt
# Can execute locally: git config commit.template .gitmessage.txt
# Dev Container automatically sets this configuration.

feat(cli): add new filter option
fix(io): handle empty file case
docs: improve API documentation
refactor: extract common utility functions
```

## Notes

- Commit messages should be concise and clear
- Each commit should do one thing
- Avoid overly broad descriptions
- Use English for commit messages
