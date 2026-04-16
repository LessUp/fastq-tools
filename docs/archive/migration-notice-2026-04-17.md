# Documentation Migration Notice

> **Date**: 2026-04-17  
> **Status**: Complete

## Migration Summary

As of 2026-04-17, all specification and decision documents have been migrated to the new `/specs` directory structure following Spec-Driven Development (SDD) conventions.

## Migrated Documents

### Old Location → New Location

| Old Path | New Path | Type |
|----------|----------|------|
| `docs/specs/steering/product.md` | `specs/product/fastq-processing.md` | Product Spec |
| `docs/specs/steering/tech.md` | `specs/rfc/0002-toolchain-policy.md` | RFC |
| `docs/specs/steering/structure.md` | `specs/rfc/0001-core-architecture.md` | RFC |
| `docs/specs/advanced-cpp-tooling/*` | `specs/rfc/0005-advanced-tooling.md` | RFC |
| `docs/specs/benchmark-system/*` | `specs/rfc/0003-benchmark-system.md` | RFC |
| `docs/specs/memory-pool/*` | `specs/rfc/0004-memory-pool.md` | RFC |
| `docs/decisions/toolchain-policy.md` | `specs/rfc/0002-toolchain-policy.md` | RFC |
| `docs/decisions/optimization-decisions.md` | Merged into relevant RFCs | RFC |
| `docs/decisions/project-assessment.md` | Merged into relevant RFCs | RFC |
| `docs/api/*.md` | `specs/api/core-api.md` (consolidated) | API Spec |

## Archived Directories

The following directories have been archived to `docs/archive/2026-04-17-migrated-to-specs/`:

- `docs/specs/` (old structure)
- `docs/decisions/` (old structure)

## Action Required

1. **Update bookmarks**: Update any bookmarks or links to old spec locations
2. **Use new structure**: All new spec work should use `/specs` directory
3. **Reference migration**: See this document for historical spec locations

## New Structure

```
specs/
├── product/            # Product requirements
├── rfc/                # Technical design documents
├── api/                # API specifications
├── db/                 # Data schemas
└── testing/            # Testing conventions
```

See `specs/README.md` for complete structure details.

## Rationale

This migration aligns with:
1. **Spec-Driven Development**: Clear separation of specs from general documentation
2. **GitHub Best Practices**: Standard spec directory structure
3. **Discoverability**: Numbered RFCs and clear categorization
4. **AI Agent Workflow**: Explicit spec-first workflow for AI assistants

## Questions

If you have questions about the migration or can't find a document, please:
1. Check `docs/archive/2026-04-17-migrated-to-specs/`
2. Open an issue in the repository
3. Contact the FastQTools core team
