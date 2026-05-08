# Testing Specification: FastQTools

> **Status**: Active
> **Last Updated**: 2026-04-28
> **Related**: [Product Spec](../product/fastq-processing.md), [Architecture-0001](../architecture/0001-core-architecture.md)

## Overview

This document defines the testing strategy and conventions for FastQTools. Testing ensures correctness, performance, and reliability across all modules.

## Test Organization

```
tests/
├── unit/                    # Unit tests (GTest)
│   ├── common/              # Common utilities tests
│   ├── io/                  # I/O module tests
│   ├── processing/          # Processing pipeline tests
│   ├── statistics/          # Statistics module tests
│   ├── config/              # Configuration tests
│   └── error/               # Error handling tests
├── integration/             # Integration tests
│   ├── pipeline/            # Pipeline integration
│   └── cli/                 # CLI integration
├── e2e/                     # End-to-end tests
│   ├── stat/                # Stat command e2e
│   ├── filter/              # Filter command e2e
│   └── shell/               # Shell-based e2e
└── utils/                   # Test utilities
    ├── fixtures/            # Test fixtures
    └── mocks/               # Mock objects
```

## Test Framework

### Google Test (GTest)

All unit and integration tests use Google Test:

```cpp
#include <gtest/gtest.h>

TEST(FastqReaderTest, ReadsSimpleFile) {
    fq::io::FastqReader reader("test_data/simple.fastq");
    auto batch = reader.readBatch(100);
    
    EXPECT_EQ(batch->count(), 10);
    EXPECT_TRUE(reader.hasNext());
}

TEST(FastqReaderTest, ThrowsOnInvalidFile) {
    EXPECT_THROW({
        fq::io::FastqReader reader("nonexistent.fastq");
    }, fq::error::IOError);
}
```

### CTest Integration

All test targets registered with CTest:

```cmake
add_fq_test(test_io
    SOURCES io/test_reader.cpp io/test_writer.cpp
    DEPENDENCIES fq_modern_io fq_common
)
```

**Running Tests**:
```bash
# Run all tests
./scripts/core/test

# Run specific test type
./scripts/core/test --unit
./scripts/core/test --integration
./scripts/core/test --e2e

# Filter by name
./scripts/core/test --filter '*reader*'

# Run with Valgrind
./scripts/core/test --valgrind

# Run with coverage
./scripts/core/test --coverage
```

## Test Categories

### 1. Unit Tests

Test individual modules in isolation.

**Conventions**:
- Test file: `tests/unit/<module>/test_<component>.cpp`
- Test suite: `TEST(<Component>Test, <Scenario>)`
- One test per scenario (arrange-act-assert)

**Example**:
```cpp
TEST(QualityPredicateTest, PassesHighQuality) {
    fq::io::FastqRecord record;
    record.quality = "IIIIIIIIII";  // High quality
    
    fq::processing::QualityPredicate pred(20);
    EXPECT_TRUE(pred.passes(record));
}

TEST(QualityPredicateTest, FailsLowQuality) {
    fq::io::FastqRecord record;
    record.quality = "!!!!!!!!!!";  // Low quality
    
    fq::processing::QualityPredicate pred(20);
    EXPECT_FALSE(pred.passes(record));
}
```

### 2. Integration Tests

Test cross-module interactions.

**Focus Areas**:
- Reader → Pipeline → Writer flow
- Configuration → Command integration
- Error propagation across layers
- Execution backend / memory policy option compatibility
- Optional signature sidecar output compatibility

**Example**:
```cpp
TEST(PipelineIntegrationTest, FullProcessing) {
    // Setup
    fq::io::FastqReader reader("test_data/input.fastq");
    fq::io::FastqWriter writer("test_data/output.fastq");
    auto pipeline = fq::processing::createPipeline(config);
    
    // Execute
    while (reader.hasNext()) {
        auto batch = reader.readBatch();
        auto processed = pipeline->execute(std::move(batch));
        writer.writeBatch(*processed);
    }
    
    // Verify
    EXPECT_TRUE(filesMatch("test_data/output.fastq", "test_data/expected.fastq"));
}
```

### 3. End-to-End Tests

Test complete CLI workflows.

**Types**:
- Shell script-based tests
- Python-based validation
- Output comparison with golden files
- Small-sample smoke validation for signature sidecar and bounded preprocessing options

**Example** (Shell):
```bash
#!/bin/bash
# tests/e2e/test_stat.sh

INPUT="test_data/sample.fastq"
OUTPUT=$(mktemp)

FastQTools stat -i "$INPUT" -o "$OUTPUT"

# Check exit code
if [ $? -ne 0 ]; then
    echo "Stat command failed"
    exit 1
fi

# Verify output contains expected fields
grep -q "total_reads" "$OUTPUT" || { echo "Missing total_reads"; exit 1; }
grep -q "gc_content" "$OUTPUT" || { echo "Missing GC content"; exit 1; }

rm "$OUTPUT"
```

### 4. Property-Based Tests

Test universal invariants across many inputs.

**Framework**: Custom property testing with GTest.

**Example**:
```cpp
TEST(PropertyBasedTest, LengthInvariant) {
    for (int i = 0; i < 100; ++i) {
        auto record = generateRandomRecord();
        EXPECT_EQ(record.length(), record.sequence.length());
    }
}
```

### 5. Performance Tests

Benchmark and validate performance targets.

**Google Benchmark**:
```cpp
static void BM_FastqReader(benchmark::State& state) {
    for (auto _ : state) {
        fq::io::FastqReader reader("test_data/large.fastq");
        while (reader.hasNext()) {
            benchmark::DoNotOptimize(reader.readBatch());
        }
    }
}
BENCHMARK(BM_FastqReader);
```

**Validation**:
- Performance targets documented in `baseline/product/fastq-processing.md`
- Regression detection via `baseline/architecture/0003-benchmark-system.md`
- Large-sample smoke validation should include bounded-memory `stat` sidecar generation and preprocessing-heavy `filter` runs

## Coverage Requirements

| Metric | Threshold |
|--------|-----------|
| Line coverage | ≥90% |
| Branch coverage | ≥85% |
| Function coverage | ≥95% |

**Running Coverage**:
```bash
./scripts/core/build --coverage
./scripts/core/test --coverage
./scripts/tools/coverage-report
```

## Sanitizer Testing

Sanitizer coverage is enforced in CI for mainline-quality changes:

| Sanitizer | CI Job | Failure Action |
|-----------|--------|---------------|
| ASan | `clang-asan` preset | Treat as release blocker |
| TSan | `clang-tsan` preset | Treat as release blocker |
| UBSan | Part of ASan job | Treat as release blocker |

**Local Testing**:
```bash
./scripts/core/build --sanitizer asan --dev
./scripts/core/test
```

## Test Data Management

### Location

- Small test data: `tools/data/` (committed to repo)
- Large test data: Generated on-demand via scripts
- Fuzzing corpus: `tools/fuzz/corpus/`

### Generation Scripts

```bash
# Generate test data
./tools/data/generate --size small   # 1K reads
./tools/data/generate --size medium  # 100K reads
./tools/data/generate --size large   # 1M reads
```

### Test Data Properties

All generated test data must:
1. Be valid FASTQ format
2. Have known, deterministic properties
3. Be reproducible (seed-based generation)
4. Include at least one real gzip FASTQ smoke dataset for CLI-level validation when available locally

## Test Naming Conventions

### Test Files

Pattern: `test_<module>.cpp`

| Module | Test File |
|--------|-----------|
| `FastqReader` | `test_io.cpp` |
| `ProcessingPipeline` | `test_processing.cpp` |
| `StatisticCalculator` | `test_statistics.cpp` |
| `Configuration` | `test_common.cpp` |

### Test Cases

Pattern: `TEST(<Component>Test, <Scenario>)`

Examples:
- `TEST(FastqReaderTest, ReadsCompressedFile)`
- `TEST(ProcessingPipelineTest, HandlesEmptyInput)`
- `TEST(StatisticCalculatorTest, CalculatesGcContent)`

### CTest Registration

Automatic via `add_fq_test()` in `tests/CMakeLists.txt`:
- CTest name: `test_<module>`
- Label: `<module>`
- Timeout: 60s (unit), 300s (integration), 600s (e2e)

## Error Testing

All error scenarios must have tests:

```cpp
TEST(FastqReaderTest, ThrowsOnFileNotFound) {
    EXPECT_THROW(fq::io::FastqReader("nonexistent.fastq"), fq::error::IOError);
}

TEST(FastqReaderTest, ThrowsOnInvalidFormat) {
    EXPECT_THROW({
        fq::io::FastqReader reader("test_data/invalid.fastq");
        reader.readBatch(100);
    }, fq::error::FormatError);
}
```

## Maintained Regression Matrix

The maintained fast regression matrix for current runtime slices and next-gen baseline additions SHALL include:

1. `filter` with `--profile highThroughput`
2. `stat` with `--profile lowMemory --memory-limit-gb 1`
3. `filter` with adapter trimming and poly-tail trimming enabled
4. `stat` with `--signature-report` enabled, including duplicate estimate and bounded `head_kmer` rows

## CI Integration

Tests run in CI on every PR:

```yaml
- name: Run tests
  run: ./scripts/core/test --coverage

- name: Upload coverage
  uses: codecov/codecov-action@v3
  with:
    file: ./coverage.xml
```

## Test-Driven Development Workflow

1. **Write failing test** for desired behavior
2. **Implement minimal code** to pass test
3. **Refactor** while keeping tests green
4. **Add edge case tests** for robustness
5. **Update specs** if behavior changes

## Related Documents

- [Product Specification](../product/fastq-processing.md)
- [Architecture-0001: Core Architecture](../architecture/0001-core-architecture.md)
- [Architecture-0005: Advanced Tooling](../architecture/0005-advanced-tooling.md)
- [Testing Guide](../../../docs/dev/testing.md)
