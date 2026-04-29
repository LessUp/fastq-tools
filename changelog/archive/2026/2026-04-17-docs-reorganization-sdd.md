# Documentation Reorganization — Spec-Driven Development

> **Date**: 2026-04-17  
> **Type**: chore(docs)  
> **Scope**: documentation, specs, project-structure

## Objective

Reorganize project documentation to follow Spec-Driven Development (SDD) methodology and GitHub community best practices.

## Major Changes

### 1. New `/specs` Directory Structure

Created dedicated `/specs` directory at project root with standardized structure:

```
specs/
├── product/            # Product requirements
│   └── fastq-processing.md
├── rfc/                # Technical design documents
│   ├── 0001-core-architecture.md
│   ├── 0002-toolchain-policy.md
│   ├── 0003-benchmark-system.md
│   ├── 0004-memory-pool.md
│   └── 0005-advanced-tooling.md
├── api/                # API specifications
│   └── core-api.md
├── db/                 # Data schemas
│   └── schema.md
└── testing/            # Testing conventions
    └── test-strategy.md
```

### 2. Migrated Existing Documents

- **Old `docs/specs/*`**: Migrated to `/specs/rfc/` and `/specs/product/`
- **Old `docs/decisions/*`**: Migrated to `/specs/rfc/`
- **Old `docs/api/*`**: Consolidated into `/specs/api/core-api.md`
- **Archive**: Old documents moved to `docs/archive/2026-04-17-migrated-to-specs/`

### 3. Updated Agent Rules

- **AGENTS.md**: Added SDD workflow with 4-step process (Review → Spec-First → Implement → Test)
- **CLAUDE.md**: Added SDD workflow and updated spec references
- Both files now mandate specs as Single Source of Truth

### 4. README Restructuring

- **README.md**: Now English version (default)
- **README.zh-CN.md**: Chinese version with links to English
- Both reference new `/specs` directory

### 5. CONTRIBUTING.md Update

- Added SDD workflow section
- Spec-first contribution guidelines
- Spec categories documentation

### 6. Documentation Index

- Updated `docs/README.md` to reflect new structure
- Added migration notice in `docs/archive/`

## Impact Scope

- **Developers**: Must follow spec-first workflow for contributions
- **AI Agents**: Must review specs before generating code
- **Users**: No impact; documentation more discoverable
- **CI/CD**: No changes to build or test processes

## Validation

- All spec documents created and properly structured
- Old documents archived with migration notice
- README links updated and functional
- AGENTS.md and CLAUDE.md workflows complete
- CONTRIBUTING.md SDD guidelines added

## Related Files

- `/specs/README.md` - New specs directory guide
- `docs/archive/migration-notice-2026-04-17.md` - Migration details
- `AGENTS.md` - Updated with SDD workflow
- `CLAUDE.md` - Updated with SDD workflow
- `CONTRIBUTING.md` - Updated with SDD guidelines
- `README.md` - Now English default with Chinese link
- `README.zh-CN.md` - Chinese version with English link

## Notes

This reorganization aligns with:
1. Spec-Driven Development best practices
2. GitHub community documentation standards
3. AI agent workflow requirements
4. Open source project conventions
