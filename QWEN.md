# FastQTools - QWEN Context File

## Project Overview

**FastQTools** is a high-performance toolkit for processing FASTQ files (bioinformatics sequencing data) built with **modern C++23**. It provides extreme performance (up to 1.7 billion reads/second) through parallel TBB pipelines, zero-copy I/O using `std::string_view`, and production-ready quality with comprehensive sanitizers, fuzzing, and CI/CD validation.

### Key Features
- **`stat` command**: Comprehensive FASTQ file statistics (read count, length distribution, base composition, GC content, Q20/Q30)
- **`filter` command**: Flexible read filtering and quality trimming (quality threshold, length range, N-base ratio, trimming from ends)

### Tech Stack
- **Language**: C++23
- **Parallel Processing**: Intel oneTBB 2022.3.0
- **Build System**: CMake 3.28+ with Ninja
- **Package Manager**: Conan 2.x
- **Compression**: libdeflate 1.25 + zlib-ng 2.3.2
- **Logging**: spdlog 1.17.0
- **CLI Parsing**: cxxopts 3.1.1
- **Testing**: GTest 1.14+

## Building and Running

### Quick Start (One-Command Build)

```bash
# Clone and build
git clone https://github.com/LessUp/fastq-tools.git
cd fastq-tools
./scripts/core/build

# Run
./build/clang-release/FastQTools --help
```

### Build Commands

```bash
# Default build (Clang Release)
./scripts/core/build

# Development mode (Debug + verbose)
./scripts/core/build --dev

# GCC Debug build
./scripts/core/build --compiler gcc --type Debug

# Sanitizer builds
./scripts/core/build --sanitizer asan
./scripts/core/build --sanitizer tsan

# Coverage build
./scripts/core/build --coverage
```

### CMake Presets

```bash
# Configure with preset
cmake --preset clang-release
# or
cmake --preset gcc-debug

# Build
cmake --build --preset clang-release
```

Available presets:
- `clang-debug`, `clang-release`
- `gcc-debug`, `gcc-release`, `gcc-relwithdebinfo`
- `clang-asan`, `clang-tsan`
- `coverage`

### Running Commands

```bash
# Statistical analysis
FastQTools stat -i input.fastq.gz -o report.txt

# Filtering with quality trimming
FastQTools filter -i input.fq.gz -o filtered.fq.gz \
  --min-quality 20 --min-length 50

# JSON output
FastQTools stat -i input.fastq.gz -o report.json --format json
```

### Testing

```bash
# Run all tests
./scripts/core/test

# Unit tests only
./scripts/core/test --unit

# Integration tests
./scripts/core/test --integration

# End-to-end tests
./scripts/core/test --e2e

# With coverage
./scripts/core/test --coverage

# Filter specific test
./scripts/core/test --filter '^test_io$'
```

### Linting and Formatting

```bash
# Check code style
./scripts/core/lint check

# Auto-format code
./scripts/core/lint format

# Static analysis (clang-tidy)
./scripts/core/lint tidy -b build/clang-debug

# Fix clang-tidy issues
./scripts/core/lint tidy-fix -b build/clang-debug
```

## Development Conventions

### Architecture Philosophy

The project follows **Spec-Driven Development (SDD)**. All implementations must be based on specification documents in `/specs/`:
- `/specs/product/` - Product requirements and acceptance criteria
- `/specs/rfc/` - Technical design documents (architecture decisions)
- `/specs/api/` - API specifications
- `/specs/db/` - Data models
- `/specs/testing/` - Testing strategies

### Code Style

- **C++ Standard**: C++23 (no compiler extensions)
- **Indentation**: 4 spaces, UTF-8, LF line endings
- **Column Width**: 100 characters
- **Pointer Alignment**: Left (`int* ptr`)
- **Brace Style**: Attached (Allman style for control structures)
- **Trailing Return Types**: Preferred (`auto foo() -> int`)
- **Naming Conventions**:
  - Classes/Structs: `PascalCase` (e.g., `FastqBatch`)
  - Functions/Methods: `camelCase` (e.g., `validateLengths()`)
  - Variables/Parameters: `camelCase` (e.g., `totalReads`)
  - Private/Protected Members: `camelCase_` (e.g., `config_`)
  - Constants: `kCamelCase` (e.g., `kDefaultBatchSize`)
  - Enum Values: `PascalCase` (e.g., `CompressionType::Gzip`)
  - Namespaces: `lower_case` (e.g., `fq::processing`)

### Project Structure

```
fastq-tools/
├── include/fqtools/          # Public API headers (stable)
│   ├── io/                   # I/O interfaces (fastq_reader, fastq_writer)
│   ├── processing/           # Processing pipeline interfaces
│   ├── statistics/           # Statistics interfaces
│   ├── benchmark/            # Benchmark interfaces
│   └── common/               # Common types
├── src/                      # Implementation code
│   ├── cli/                  # CLI entry point and commands
│   ├── io/                   # FASTQ I/O implementations
│   ├── processing/           # Pipeline and mutators
│   ├── statistics/           # Statistical calculators
│   ├── config/               # Configuration parsing
│   └── error/                # Error handling
├── tests/                    # Test suite
│   ├── unit/                 # Unit tests (mirrors src/ structure)
│   ├── integration/          # Integration tests
│   └── e2e/                  # End-to-end tests (CLI)
├── tools/                    # Development tools
│   ├── benchmark/            # Performance benchmarks
│   └── fuzz/                 # Fuzzing harnesses
├── specs/                    # Specification documents (SDD)
├── docs/                     # MkDocs documentation site
├── scripts/core/             # Build, test, lint scripts
├── cmake/                    # CMake modules
└── config/                   # Tool configurations
```

### Error Handling

- **Exception Base**: `fq::error::FastQException`
- **Exception Types**: `IOError`, `FormatError`, `ConfigurationError`
- **Macros**: Use `FQ_THROW_IO_ERROR`, `FQ_THROW_FORMAT_ERROR`, `FQ_THROW_CONFIG_ERROR`
- **Logging**: Use `fq::logging::trace/debug/info/warn/error/critical` (fmt-style format strings)
- **Never swallow exceptions** - catch and log at CLI boundaries

### Testing Practices

- Unit tests in `tests/unit/<module>/` mirror source structure
- Integration tests in `tests/integration/` for cross-module validation
- E2E tests in `tests/e2e/` for CLI testing
- Tests registered automatically via `add_fq_test()` in CMakeLists.txt
- CTest integration: `ctest --test-dir build/clang-debug -R '^test_name$'`

### Change Management

- Every change must add a changelog entry in `changelog/YYYY-MM-DD-<slug>.md`
- Changelog entries must include: goal, main changes, impact area, verification method
- Use Conventional Commits: `feat|fix|docs|refactor|test|build|ci|chore(scope): subject`

### Header File Organization

- Public headers: `include/fqtools/<module>/`
- Internal headers: `src/<module>/` (use quotes for includes)
- Use `#pragma once` for header guards
- Avoid `using namespace` in headers
- Let `clang-format` manage include order

## Key Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| Intel oneTBB | 2022.3.0 | Parallel pipeline processing |
| zlib-ng | 2.3.2 | Gzip compression/decompression |
| libdeflate | 1.25 | High-performance deflate |
| cxxopts | 3.1.1 | Command-line argument parsing |
| spdlog | 1.17.0 | Async logging framework |
| fmt | 12.1.0 | Formatting library (header-only) |
| nlohmann_json | 3.11.3 | JSON processing |
| GTest | 1.14+ | Unit testing framework |

## Build Artifacts

- **Build Directory**: `build/<compiler>-<type-lower>` (e.g., `build/clang-debug`)
- **Install Directory**: `install/<preset-name>`
- **Binary Output**: `build/clang-release/FastQTools`
- **Test Binaries**: `build/clang-debug/tests/unit/<test_name>`

## Recommended Compiler

- **Primary**: Clang 21 with libc++
- **Alternative**: GCC 15
- **Note**: Default build uses Clang Release mode

## CI/CD

GitHub Actions workflows:
- **ci.yml**: Build and test matrix (GCC/Clang × Debug/Release)
- **quality.yml**: Format check, ASan/TSan tests, coverage
- **pages.yml**: MkDocs documentation deployment
- **benchmark.yml**: Performance benchmark tracking
- **release.yml**: Release packaging
- **valgrind.yml**: Memory checking

## Docker

Production-ready Docker support:
```bash
# Pull from Docker Hub
docker pull lessup/fastqtools:latest

# Run with volume mount
docker run -v /your/data:/data lessup/fastqtools:latest \
  FastQTools stat -i /data/input.fastq.gz -o /data/report.txt
```

## Documentation

- **Online Docs**: https://lessup.github.io/fastq-tools/
- **MkDocs Config**: `mkdocs.yml`
- **Docs Source**: `docs/` (bilingual: Chinese/English)
- **Build Docs**: `pip install -r requirements-docs.txt && mkdocs serve`

## Important Notes

1. **Spec-Driven**: Always check `/specs/` before implementing features
2. **Zero-Copy Design**: Project emphasizes `std::string_view`-based processing - avoid unnecessary string copies
3. **Parallel Processing**: Uses `tbb::parallel_pipeline` - don't introduce serial bottlenecks in hot paths
4. **Public API Stability**: `include/fqtools/` headers are stable API - confirm before modifying
5. **Minimum Changes**: Prefer smallest correct change over refactoring unrelated code
6. **Update Tests**: Always update or add tests when changing behavior
7. **Changelog Required**: Every modification needs a changelog entry

## Useful Paths

- Build script: `scripts/core/build`
- Test script: `scripts/core/test`
- Lint script: `scripts/core/lint`
- Main CMakeLists: `CMakeLists.txt`
- Conan recipe: `conanfile.py`
- Agent guide: `AGENTS.md`
- Changelog directory: `changelog/`
- Specifications: `specs/`
