# RFC-0002: Toolchain Policy

> **Status**: Active  
> **Created**: 2026-04-17  
> **Last Updated**: 2026-04-17  
> **Authors**: FastQTools Core Team  
> **Supersedes**: `docs/decisions/toolchain-policy.md`

## Context

FastQTools requires a consistent, reproducible toolchain across development, CI, and production environments to eliminate "works on my machine" issues and ensure binary compatibility.

## Decision

### Unified Toolchain Strategy

**All environments (development, CI, production) use the same compiler versions**, packaged via Docker to isolate users from compiler dependencies.

### Version Specifications

| Component | Unified Version | Minimum Compatible | Notes |
|-----------|----------------|-------------------|-------|
| **GCC** | 15.x | 11.0 | CI compatibility check: 14.x (allow-failure) |
| **Clang/LLVM** | 21 | 12.0 | CI compatibility check: 19 (allow-failure) |
| **C++ Standard** | C++23 | C++23 | Required for all code |
| **CMake** | 4.x (Docker) | 3.28 | `cmake_minimum_required(VERSION 3.28)` |
| **Conan** | 2.24.0 | 2.0 | Locked in Dockerfile |
| **Ninja** | Latest system | 1.10 | Not locked, use system package |

### Docker Image Strategy

| Purpose | Image | Rationale |
|---------|-------|-----------|
| **Build (unified)** | `gcc:15.2-bookworm` | Dev, CI, prod all use same base image |
| **Runtime** | `debian:bookworm-slim` | Minimal footprint (~52MB), security surface |

**Tag Strategy**:
```
fastqtools:latest          ← GCC 15 build, debian:bookworm-slim runtime (recommended)
fastqtools:<version>       ← Version-locked, e.g., fastqtools:3.1.0
```

### Rationale

1. **Docker Distribution**: Users receive pre-compiled binaries; compiler version is transparent to them
2. **Environment Consistency**: Eliminates "dev compiles, prod fails" issues
3. **GCC 15 Value**: Latest optimizer, mature C++23 support, bugfix release
4. **Single Configuration**: Lower cognitive load and error probability than maintaining multiple configs

### Why Not Other Options

**Not Ubuntu**:
- Larger image size, no benefit for this project
- Ubuntu 24.04 itself is based on Debian Bookworm, binary compatible anyway
- Can create Ubuntu variant if bioinformatics community demands

**Not Alpine**:
- musl libc vs glibc ABI incompatibility
- TBB, libdeflate may have issues or performance degradation on musl

### CI Matrix Configuration

```yaml
strategy:
  matrix:
    include:
      # ===== Primary (must pass) =====
      - compiler: gcc
        version: "15"
        preset: gcc-release

      - compiler: clang
        version: "21"
        preset: clang-release

      # ===== Sanitizers (must pass) =====
      - compiler: clang
        version: "21"
        preset: clang-asan

      - compiler: clang
        version: "21"
        preset: clang-tsan

      # ===== Compatibility checks (allow-failure) =====
      - compiler: gcc
        version: "14"
        preset: gcc-release
        allow-failure: true

      - compiler: clang
        version: "19"
        preset: clang-release
        allow-failure: true
```

### Version Locking Rules

1. **Must use versioned image tags** (e.g., `gcc:15.2-bookworm`), never `latest`
2. **Must document** selection rationale and last review date in Dockerfiles
3. **Runtime image** follows Debian major version upgrades (Bookworm → Trixie), expect 2025-2026 review

### Upgrade Policy

| Component | Review Cycle | Notes |
|-----------|--------------|-------|
| GCC | Within 1 month of new stable release | Check C++23 feature maturity |
| Clang | Follow LLVM stable release schedule | Within 1 month |
| Minimum compatible | Every 2 years | Follow mainstream LTS distros |
| CMake minimum | As needed | Currently 3.28 (Ubuntu 24.04 line) |
| Conan | After each release | Verify profile compatibility |
| Runtime base image | Debian major version | Bookworm EOL: 2028 |

### Review Process

1. **Bi-annual review** (January / July) of all toolchain versions
2. **Review scope**: Compiler releases, Docker base image security updates, Conan compatibility
3. **Review results** recorded in `changelog/`
4. **Version changes** must update this document and all related files

### Consistency Checklist

The following files must remain consistent with this specification:
- [ ] `AGENTS.md` / `CLAUDE.md`
- [ ] `config/conan/profile-gcc`
- [ ] `config/conan/profile-clang`
- [ ] `docker/Dockerfile.dev`
- [ ] `docker/Dockerfile.prod`
- [ ] `docker/Dockerfile.deploy`
- [ ] `CMakeLists.txt`
- [ ] `CMakePresets.json`
- [ ] `docs/dev/build.md`

## Consequences

### Positive
- Reproducible builds across environments
- Docker isolates users from compiler dependencies
- Single configuration reduces maintenance burden
- Sanitizer results consistent between dev and CI

### Negative
- Requires Docker for development
- Users building from source need specific compiler versions
- Docker layer caching can cause non-deterministic builds (mitigated by apt snapshot pinning)

### Risks & Mitigations

| Risk | Mitigation |
|------|-----------|
| Clang 21 via `llvm.sh` may pull different commits | Cache Docker layers in CI, pin apt snapshot dates |
| GCC 15 not available on older distros | Docker distribution, users get binaries |
| Conan 2.x API changes | Lock version in Dockerfile, review after releases |

## Related Documents

- [Product Specification](../product/fastq-processing.md)
- [RFC-0001: Core Architecture](0001-core-architecture.md)
- [Build Guide](../../docs/dev/build.md)
