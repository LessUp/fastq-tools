# Changelog

All notable changes to this project are documented here.

## Latest Version

For the complete changelog, visit the GitHub repository:

- [Full Changelog](https://github.com/LessUp/fastq-tools/blob/master/CHANGELOG.md)
- [Detailed Change Records](https://github.com/LessUp/fastq-tools/tree/master/changelog/)

## Change Format

We use date-based change records:

```
changelog/
├── 2026-04-17-optimize-directory-structure.md
├── 2026-04-17-fix-github-workflows.md
└── ...
```

Each change file contains:
- Goals and scope
- Main changes
- Impact area
- Verification method

## Conventional Commits

Commit messages follow the [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
feat|fix|docs|refactor|test|build|ci|chore(scope): subject
```

Examples:
- `feat(cli): add filter command`
- `fix(io): resolve memory leak in fastq reader`
- `docs: update getting started guide`
