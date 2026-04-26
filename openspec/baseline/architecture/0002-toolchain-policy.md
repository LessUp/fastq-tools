# RFC-0002: Toolchain Policy

> **Status**: Active
> **Created**: 2026-04-17
> **Last Updated**: 2026-04-26
> **Authors**: FastQTools Core Team
> **Migrated From**: legacy toolchain decision document in the pre-OpenSpec docs tree

## Context

FastQTools needs a toolchain policy that is reproducible enough for CI and contributor handoff, but lightweight enough for a maintenance closeout repository. The previous baseline overstated uniformity by claiming a single Docker-pinned compiler stack across development, CI, and production, while the real repository already supports multiple presets, multiple Conan profiles, and local-first workflows.

The current policy must therefore describe what contributors can actually rely on:

- source compatibility floors;
- canonical local build/test/lint entry points;
- preferred Conan/CMake profiles where available;
- consistency rules for docs, presets, Dockerfiles, and workflows.

## Decision

### 1. Source Compatibility Floor

FastQTools requires:

| Component | Maintained floor | Notes |
| --- | --- | --- |
| C++ | C++23 | Required for all code |
| GCC | 11+ | Enforced by `CMakeLists.txt` |
| Clang | 12+ | Enforced by `CMakeLists.txt` |
| CMake | 3.28+ | Presets and root build configuration depend on it |
| Conan | 2.x | Project scripts and dependency setup assume Conan 2 |

These are the compatibility floors. They are not the same thing as the repo's preferred profiles.

### 2. Canonical Local Workflow

The authoritative local entry points are:

```bash
./scripts/core/build
./scripts/core/test
./scripts/core/lint
./scripts/core/install-deps
```

The canonical local development posture is:

- use CMake presets;
- use `clang-debug` for local editor integration and compile commands;
- use `clangd + compile_commands.json` for C++ navigation and diagnostics;
- keep Docker optional for local work rather than mandatory.

### 3. Preferred Profiles and Presets

FastQTools keeps preferred Conan profiles in:

- `config/conan/profile-gcc` → GCC 15 + `libstdc++11`
- `config/conan/profile-clang` → Clang 21 + `libc++`

These profile files express the repository's preferred target configurations where those toolchains are available. They do **not** mean every local machine or CI runner must literally use those exact compiler binaries.

In practice:

- local scripts may adapt to the installed Clang toolchain while preserving the preset/profile intent;
- the verified local baseline for this repository currently includes Linux + Clang 18 + libc++;
- CI should favor signal and maintainability over maximal version-matrix breadth.

### 4. CMake Preset Policy

The maintained preset families are:

- `gcc-debug`
- `gcc-release`
- `gcc-relwithdebinfo`
- `clang-debug`
- `clang-release`
- sanitizer presets (`clang-asan`, `clang-tsan`, `clang-ubsan`, `clang-msan`)
- `coverage`

Rules:

1. Local LSP and editor tooling should target `clang-debug`.
2. Release-oriented packaging should use a release preset rather than ad-hoc directories.
3. New build flows must integrate through presets and `scripts/core/*`, not custom wrapper scripts.

### 5. Dependency Policy

The maintained dependency recipes are:

- `config/dependencies/conanfile.py` for project build/test consumption
- root `conanfile.py` for package metadata and installation/export use

If dependency versions or options change, both recipes must be reviewed together. Divergence between them is treated as technical debt and must be intentional, documented, and minimal.

### 6. Consistency Rules

The following must stay aligned whenever toolchain-facing changes are made:

- `CMakeLists.txt`
- `CMakePresets.json`
- `config/conan/profile-gcc`
- `config/conan/profile-clang`
- `config/dependencies/conanfile.py`
- `conanfile.py`
- `docker/` toolchain-related Dockerfiles
- `docs/dev/build*.md`
- AI governance docs that describe local tooling (`AGENTS.md`, `CLAUDE.md`, Copilot instructions)

### 7. Review Policy

Toolchain statements must be reviewed when one of the following changes:

1. a preferred compiler profile is updated;
2. local scripts change how presets or Conan profiles are resolved;
3. CI changes its primary build/test presets;
4. public docs or AI instructions change contributor setup guidance.

## Consequences

### Positive

- The baseline now matches the repository's real local-first workflow.
- Contributors can distinguish compatibility floors from preferred target profiles.
- Tooling guidance is simpler to keep in sync across docs, scripts, and AI instructions.

### Negative

- The repository no longer claims a single literal compiler version across every environment.
- Profile files still introduce some cognitive overhead because both preferred targets and compatibility floors exist.

### Risks & Mitigations

| Risk | Mitigation |
| --- | --- |
| Docs claim a version the scripts do not actually use | Treat cross-file version alignment as part of every toolchain change |
| Preferred Clang profile and installed local Clang differ | Keep compatibility floor explicit; document local verified baseline separately |
| Dependency recipes drift apart | Review both Conan recipes together whenever versions or options change |

## Related Documents

- [Product Specification](../product/fastq-processing.md)
- [Core Architecture](0001-core-architecture.md)
- [Build Guide](../../../docs/dev/build.md)
- [Local Tooling Strategy](../../../docs/dev/local-tooling.md)
