# RFC-0005: Advanced C++ Tooling

> **Status**: Active  
> **Created**: 2026-04-17  
> **Last Updated**: 2026-04-23  
> **Authors**: FastQTools Core Team  
> **Migrated From**: legacy advanced C++ tooling design document in the pre-OpenSpec docs tree

## Context

FastQTools requires comprehensive development tooling integration to ensure code quality, detect issues early, and maintain high reliability standards. This architecture specification defines the tooling ecosystem.

## Decision

### Tool Categories

#### 1. Sanitizer Integration

Runtime error detection via compiler Sanitizers:

| Sanitizer | Purpose | Build Flag |
|-----------|---------|-----------|
| ASan | Memory errors (use-after-free, buffer overflow) | `--sanitizer asan` |
| TSan | Data races | `--sanitizer tsan` |
| UBSan | Undefined behavior | `--sanitizer ubsan` |
| MSan | Uninitialized memory (Clang only) | `--sanitizer msan` |

**Build System Integration**:
```cmake
option(ENABLE_SANITIZERS "Enable runtime sanitizers" OFF)
if(ENABLE_SANITIZERS)
    target_compile_options(target PRIVATE -fsanitize=${SANITIZER_LIST})
    target_link_options(target PRIVATE -fsanitize=${SANITIZER_LIST})
endif()
```

#### 2. Valgrind Integration

Dynamic analysis for deep memory profiling:

| Tool | Purpose | Script |
|------|---------|--------|
| Memcheck | Memory leak detection | `./scripts/tools/valgrind-memcheck` |
| Cachegrind | Cache performance | `./scripts/tools/valgrind-cachegrind` |

**Usage**:
```bash
# Run tests with Valgrind
./scripts/core/test --valgrind

# Memcheck analysis
./scripts/tools/valgrind-memcheck

# Cache performance
./scripts/tools/valgrind-cachegrind
```

#### 3. Static Analysis

| Tool | Purpose | Script |
|------|---------|--------|
| clang-tidy | C++ best practices, bug detection | `./scripts/core/lint tidy` |
| Cppcheck | Deep static analysis | `./scripts/core/lint cppcheck` |
| IWYU | Include dependency analysis | `./scripts/core/lint iwyu` |

**Usage**:
```bash
# Run clang-tidy
./scripts/core/lint tidy -b build/clang-debug

# Run all checks
./scripts/core/lint all -b build/clang-debug

# Auto-fix issues
./scripts/core/lint tidy-fix -b build/clang-debug
```

#### 4. Code Coverage

Coverage analysis via gcov/LCOV:

```bash
# Build with coverage
./scripts/core/build --coverage --dev

# Run tests and generate report
./scripts/core/test --coverage
./scripts/tools/coverage-report

# Generate XML for CI
./scripts/tools/coverage-report --xml
```

**Coverage Thresholds**:
- Line coverage: ≥90%
- Branch coverage: ≥85%
- Function coverage: ≥95%

#### 5. Fuzz Testing

LibFuzzer integration for boundary testing:

```bash
# Build and run fuzzers
./scripts/tools/run-fuzzer --build

# Run specific fuzzer for 300 seconds
./scripts/tools/run-fuzzer parser -t 300
```

**Fuzz Target Structure**:
```cpp
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // Parse random FASTQ data
    // Should not crash
    return 0;
}
```

### CI/CD Integration

All quality tools integrated into CI/CD:

| Check | Trigger | Failure Action |
|-------|---------|---------------|
| Static analysis | Every significant change | Treat as release blocker |
| ASan tests | Every significant change | Treat as release blocker |
| Coverage report | As needed | Investigate if below threshold |
| Full Valgrind | Daily/weekly | Alert maintainers |
| Fuzz testing | Daily | Report crashes |

### Correctness Properties

#### Property 1: Sanitizer Detection

*For any* code with memory errors (use-after-free, buffer overflow), ASan SHALL detect and report the error with stack trace and source location.

#### Property 2: Data Race Detection

*For any* code with data races, TSan SHALL detect and report the race with thread information.

#### Property 3: Coverage Accuracy

*For any* test suite execution with coverage enabled, the generated coverage report SHALL accurately reflect executed lines, branches, and functions.

#### Property 4: Fuzz Test Stability

*For any* valid FASTQ input (compressed or uncompressed), the fuzzer SHALL NOT crash when processing the input.

### Error Handling

| Tool | Error | Handling |
|------|-------|----------|
| Sanitizers | Detection | Print detailed report with stack trace, exit with error |
| Valgrind | Leak detected | Print leak details, return non-zero exit code |
| Static analysis | Issue found | Print issue details, return non-zero exit code (CI mode) |
| Coverage | Below threshold | Print coverage summary, return non-zero exit code |
| Fuzzing | Crash found | Save crash input to corpus, report stack trace |

### Testing Strategy

1. **Sanitizer Tests**: Known error cases, verify detection accuracy
2. **Coverage Tests**: Simple modules, verify coverage accuracy
3. **Fuzz Tests**: Corpus-based testing, regression prevention
4. **Integration Tests**: CI workflow validation, tool chaining

## Consequences

### Positive
- Early detection of memory errors and bugs
- Comprehensive code quality assurance
- Automated regression prevention
- Developer-friendly tooling via unified scripts

### Negative
- Sanitizers add runtime overhead (2-3x slowdown)
- Fuzz testing requires corpus maintenance
- Static analysis may have false positives

## Related Documents

- [RFC-0002: Toolchain Policy](0002-toolchain-policy.md)
- [Quality Tools Guide](../../../docs/dev/quality-tools.md)
- [Testing Guide](../../../docs/dev/testing.md)
