# FastQTools Tech Stack

## Language & Standards

- C++20 (required)
- Compilers: GCC 11+ or Clang 19+ (Clang preferred for CI/dev)

## Build System

- CMake 3.20+
- Conan 2.x for dependency management
- Ninja (recommended generator)

## Dependencies

| Library | Purpose |
|---------|---------|
| Intel oneTBB | Parallel pipeline processing |
| spdlog | Logging |
| fmt | String formatting |
| cxxopts | CLI argument parsing |
| zlib/bzip2/xz | Compression support |
| libdeflate | Optimized gzip decompression |
| GoogleTest | Unit testing |

## Common Commands

### Build

```bash
# Default build (Clang + Release)
./scripts/core/build

# Development build (Debug)
./scripts/core/build --dev

# Specific compiler/type
./scripts/core/build -c gcc -t Debug

# With sanitizers
./scripts/core/build --sanitizer asan
```

### Test

```bash
# All tests
./scripts/core/test

# Unit tests only
./scripts/core/test --unit

# With coverage
./scripts/core/test --coverage

# Filter specific tests
./scripts/core/test --filter "*config*"
```

### Code Quality

```bash
# Format code
./scripts/core/lint format

# Check formatting
./scripts/core/lint check

# Static analysis
./scripts/core/lint tidy

# All checks
./scripts/core/lint all
```

### Dependencies

```bash
# Install dev dependencies
./scripts/core/install-deps

# Install all dependencies
./scripts/core/install-deps --all
```

## Build Directories

- `build-clang-release/` - Clang Release builds
- `build-clang-debug/` - Clang Debug builds
- `build-gcc-release/` - GCC Release builds
- `build-gcc-debug/` - GCC Debug builds
