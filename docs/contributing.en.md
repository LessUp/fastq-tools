# Contributing Guide

Welcome to contribute to the FastQTools project!

## Quick Start

For detailed contributing guidelines, please see [CONTRIBUTING.md](https://github.com/LessUp/fastq-tools/blob/master/CONTRIBUTING.md) in the project root.

## Ways to Contribute

### 1. Report Issues

- Use [GitHub Issues](https://github.com/LessUp/fastq-tools/issues) to report bugs or request features
- Provide detailed reproduction steps and environment information

### 2. Submit Code

```bash
# 1. Fork the repository
# 2. Create a feature branch
git checkout -b feat/my-feature

# 3. Write code and tests
# 4. Run validation
./scripts/core/lint format
./scripts/core/test

# 5. Commit and push
git commit -m "feat: add my feature"
git push origin feat/my-feature

# 6. Create a Pull Request
```

### 3. Improve Documentation

- Fix spelling/grammar errors
- Add usage examples
- Improve guides and tutorials
- Translate documentation

### 4. Code Review

- Review open Pull Requests
- Provide constructive feedback
- Verify tests pass

## Development Setup

```bash
# Install dependencies
./scripts/core/install-deps

# Build the project
./scripts/core/build

# Run tests
./scripts/core/test
```

## Code Standards

- Follow the [C++ Coding Standards](dev/coding-standards.en.md)
- Format code with `./scripts/core/lint format`
- Add necessary tests
- Update related documentation

## Commit Convention

Use [Conventional Commits](https://www.conventionalcommits.org/) format:

```
feat|fix|docs|refactor|test|build|ci|chore(scope): subject
```

## Code of Conduct

Please follow the [Code of Conduct](https://github.com/LessUp/fastq-tools/blob/master/CODE_OF_CONDUCT.md).

Thank you for your contribution! 🎉
