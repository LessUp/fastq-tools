# Documentation

This directory contains development and user documentation for FastQTools.

> **Note**: Specification documents have been moved to `/specs` at the project root. See [specs/README.md](../../specs/README.md) for the new structure.

## Directory Structure

```
docs/
├── guide/                # User guides and tutorials
│   ├── getting-started.md       # Quick start guide
│   ├── cli-reference.md         # CLI command reference
│   ├── configuration.md         # Configuration guide
│   └── deployment.md            # Deployment guide
├── dev/                  # Developer documentation
│   ├── architecture.md          # Architecture overview
│   ├── design.md                # Design principles
│   ├── build.md                 # Build instructions
│   ├── testing.md               # Testing guide
│   ├── coding-standards.md      # Coding conventions
│   ├── quality-tools.md         # Quality tooling guide
│   ├── devcontainer.md          # DevContainer setup
│   ├── migration.md             # Migration guides
│   └── benchmark-guide.md       # Benchmark guide
├── api/                  # API reference (detailed)
│   ├── overview.md              # API overview
│   ├── core.md                  # Core module API
│   ├── io.md                    # I/O module API
│   ├── processing.md            # Processing module API
│   └── statistics.md            # Statistics module API
├── performance/          # Performance reports
│   └── benchmark-report.md      # Latest benchmark report
├── benchmark-reports/    # Historical benchmark data
│   ├── latest.md                # Latest summary
│   └── *.json                   # Historical results
├── archive/              # Archived documents
│   └── 2026-04-17-migrated-to-specs/  # Old specs and decisions
└── assets/               # Static assets
    ├── css/                     # Custom stylesheets
    └── javascripts/             # Custom scripts
```

## Document Types

### User Guides (`guide/`)

Target audience: **End users** of FastQTools CLI.

- Getting started tutorials
- CLI command reference
- Configuration examples
- Deployment instructions

### Developer Docs (`dev/`)

Target audience: **Contributors** to FastQTools.

- Architecture and design decisions
- Build and test instructions
- Coding standards
- Development workflow

### API Reference (`api/`)

Target audience: **Developers** integrating FastQTools library.

- Detailed function signatures
- Usage examples
- Parameter descriptions
- Return value specifications

> For interface specifications (the Single Source of Truth), see `/specs/api/` instead.

### Performance Reports (`performance/`, `benchmark-reports/`)

Target audience: **Maintainers** monitoring performance.

- Latest benchmark results
- Historical performance data
- Regression analysis

## Related Documents

- **[Specifications](../../specs/)**: Product requirements, RFCs, API specs
- **[AGENTS.md](../../AGENTS.md)**: Agent workflow guide
- **[CLAUDE.md](../../CLAUDE.md)**: AI assistant guide
- **[CONTRIBUTING.md](../../CONTRIBUTING.md)**: Contribution guidelines
- **[README.md](../../README.md)**: Project overview
