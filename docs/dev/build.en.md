# Build Guide

## System Requirements

- **Compiler**: GCC 15 / modern Clang (unified for development and production; local Clang builds automatically adapted by scripts to installed version via Conan)
- **CMake**: 3.28+
- **Version Specification**: See [`docs/decisions/toolchain-policy.md`](../decisions/toolchain-policy.md)
- **Parallel Library**: Intel oneTBB (required, pipeline implementation depends on it)
- **Compression Libraries**: zlib / bzip2 / xz
- **Memory**: Recommended 4GB+ RAM
- **Storage**: 1GB+ available space

## Quick Build

```bash
# One-command build (Clang + Release)
./scripts/core/build

# Specify compiler and configuration
./scripts/core/build --compiler gcc --type Debug

# Enable sanitizers
./scripts/core/build --sanitizer asan --dev
```

## Dependency Management

```bash
# Install dependencies (first-time setup)
./scripts/core/install-deps

# Rebuild (recommended: use script to manage build directory)
./scripts/core/build
```

> Note: Early experiments with `mimalloc` as an allocator have been completed, but the build and runtime no longer depend on it—all configuration files have removed related requirements.

## Code Quality

```bash
# Format code
./scripts/core/lint format

# Check formatting
./scripts/core/lint check

# Static analysis
./scripts/core/lint tidy
```

## Testing

```bash
# Run tests
./scripts/core/test

# Coverage testing
./scripts/core/build --coverage --dev
./scripts/core/test --coverage
```

## Tools and Debugging

### Sanitizers (Memory and Runtime Checks)

FastQTools supports multiple LLVM/GCC Sanitizers for detecting memory errors and undefined behavior:

- **AddressSanitizer (ASan)**: Detects memory leaks, buffer overflows, use-after-free, etc.
- **UndefinedBehaviorSanitizer (USan)**: Detects integer overflow, invalid type conversions, null pointer dereferences, etc.
- **ThreadSanitizer (TSan)**: Detects multi-thread data races.

Usage:

```bash
# Enable ASan (recommended in Debug mode)
./scripts/core/build --sanitizer asan --dev

# Enable TSan (note: TSan cannot be used with ASan simultaneously)
./scripts/core/build --sanitizer tsan --dev

# Enable UBSan
./scripts/core/build --sanitizer ubsan --dev
```

When running compiled programs (e.g., unit tests), Sanitizers automatically output error reports and terminate when issues are detected.

### Static Analysis

The project integrates `clang-tidy` for static code checking:

```bash
# Run clang-tidy static analysis
./scripts/core/lint tidy

# Run all checks (format + tidy + cppcheck)
./scripts/core/lint all
```
