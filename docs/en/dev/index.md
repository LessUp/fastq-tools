# Developer Guide

This guide is intended for contributors who want to develop on the FastQTools project.

## Table of Contents

- [Architecture](architecture.md) - High-level architecture overview
- [Core Design](design.md) - Core module design decisions
- [Build Guide](build.md) - Build system and toolchain
- [Testing](testing.md) - Testing framework and conventions
- [Coding Standards](coding-standards.md) - C++ coding standards
- [Local Tooling Strategy](local-tooling.md) - clangd, MCP, and plugin boundaries
- [Git Workflow](git-guidelines.md) - Branching strategy and commit conventions
- [Quality Tools](quality-tools.md) - Linting, formatting, and static analysis
- [Benchmark Guide](benchmark-guide.md) - Performance testing methodology
- [DevContainer](devcontainer.md) - Containerized development environment

## Quick Start

1. Read the [Architecture](architecture.md) to understand the overall structure
2. Follow the [Build Guide](build.md) to set up your development environment
3. Review the [Coding Standards](coding-standards.md) for code style
4. Refer to the [Testing Strategy](testing.md) for writing tests

## Development Workflow

```
1. Sync the current branch → 2. Create a branch if needed → 3. Write code and tests →
4. Run lint/test → 5. Commit → 6. Push
```

## Contributing

Please see [CONTRIBUTING.md](https://github.com/LessUp/fastq-tools/blob/master/CONTRIBUTING.md) for how to contribute code.
