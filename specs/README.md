# Specifications

This directory contains all specification documents for FastQTools, following the **Spec-Driven Development (SDD)** methodology.

> **All code implementation must be based on these specifications as the Single Source of Truth.**

## Directory Structure

```
specs/
├── product/            # Product requirements and feature definitions
│   └── fastq-processing.md    # FASTQ processing product spec
├── rfc/                # Technical design documents and architecture decisions
│   ├── 0001-core-architecture.md
│   ├── 0002-toolchain-policy.md
│   ├── 0003-benchmark-system.md
│   ├── 0004-memory-pool.md
│   └── 0005-advanced-tooling.md
├── api/                # API interface specifications
│   └── core-api.md
├── db/                 # Data model and configuration schemas
│   └── schema.md
└── testing/            # Testing strategies and conventions
    └── test-strategy.md
```

## Specification Types

### Product Specs (`product/`)

- **Purpose**: Define product features, user stories, and acceptance criteria
- **Audience**: Product managers, developers, users
- **Format**: Markdown with user stories and acceptance criteria
- **Example**: `specs/product/fastq-processing.md`

### RFCs (`rfc/`)

- **Purpose**: Technical design documents, architecture decisions, implementation strategies
- **Audience**: Developers, architects
- **Format**: Numbered files (`NNNN-slug.md`), with Context → Decision → Consequences structure
- **Example**: `specs/rfc/0001-core-architecture.md`

### API Specs (`api/`)

- **Purpose**: Public interface definitions, function signatures, data types
- **Audience**: Developers, library consumers
- **Format**: C++ code examples with interface definitions
- **Example**: `specs/api/core-api.md`

### Schema Specs (`db/`)

- **Purpose**: Configuration formats, data models, output schemas
- **Audience**: Developers, integration engineers
- **Format**: JSON schemas, field definitions, validation rules
- **Example**: `specs/db/schema.md`

### Testing Specs (`testing/`)

- **Purpose**: Testing strategies, conventions, coverage requirements
- **Audience**: Developers, QA engineers
- **Format**: Test organization, frameworks, examples
- **Example**: `specs/testing/test-strategy.md`

## Workflow

When implementing features or fixing bugs:

1. **Review Specs**: Read relevant product, RFC, and API specs first
2. **Spec-First Update**: If changing behavior, update specs before code
3. **Implementation**: Write code that is 100% compliant with specs
4. **Test Validation**: Ensure tests cover spec acceptance criteria

## Adding New Specifications

1. Choose appropriate directory based on spec type
2. Use descriptive filename (`<NNNN-slug>.md` for RFCs)
3. Follow existing spec templates and structure
4. Link related specs in "Related Documents" section
5. Update this README if adding new spec categories

## Templates

### RFC Template

```markdown
# RFC-NNNN: <Title>

> **Status**: Active  
> **Created**: YYYY-MM-DD  
> **Last Updated**: YYYY-MM-DD  
> **Authors**: <Team>

## Context

<Problem statement and background>

## Decision

<Technical decision with rationale>

## Consequences

<Positive and negative impacts>

## Related Documents

- [Link to related specs]
```

### Product Spec Template

```markdown
# Product Specification: <Feature>

> **Status**: Active  
> **Last Updated**: YYYY-MM-DD

## Overview

<Feature description>

## Target Users

<Who benefits from this feature>

## Core Features

<Feature list with descriptions>

## Acceptance Criteria

1. <Criterion 1>
2. <Criterion 2>
3. <Criterion 3>

## Related Specifications

- [Link to related specs]
```

## Related Documents

- [AGENTS.md](../AGENTS.md) - Agent workflow with SDD
- [CLAUDE.md](../CLAUDE.md) - AI assistant guide with SDD
- [CONTRIBUTING.md](../CONTRIBUTING.md) - Contribution guidelines
