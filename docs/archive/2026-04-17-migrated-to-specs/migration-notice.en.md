# Migration Notice

> This page is archived for historical reference only.

---

## Overview

**Date**: 2026-04-17

This migration moved specification documents from `docs/` to `/specs/`, following the **Spec-Driven Development (SDD)** paradigm.

---

## Migrated Content

| Original Path | New Path |
|---------------|----------|
| `docs/steering/` | `specs/product/` |
| `docs/benchmark-system/` | `specs/rfc/` |
| `docs/memory-pool/` | `specs/rfc/` |
| `docs/advanced-cpp-tooling/` | `specs/rfc/` |

---

## New Directory Structure

```
specs/
├── product/       # Product definitions & acceptance criteria
├── rfc/           # Technical design documents
├── api/           # API specifications
├── db/            # Data models & configuration specs
└── testing/       # Testing strategies & conventions
```

---

## Reference Links

- [specs/README.md](../../../specs/README.md) — Specification index
- [AGENTS.md](../../../AGENTS.md) — AI Agent workflow guide
