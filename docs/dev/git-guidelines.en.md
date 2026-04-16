# Git Commit Guidelines

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
