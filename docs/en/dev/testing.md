# Testing Strategy

FastQTools adopts a layered testing strategy, combining unit tests, integration tests, and end-to-end tests to ensure code correctness.

---

## Testing Layers

```
        /\
       /  \  E2E Tests (few, verify CLI integration)
      /----\
     /      \  Integration Tests (moderate, verify module collaboration)
    /--------\
   /          \  Unit Tests (many, verify independent logic)
  /------------\
```

### Directory Structure

```
tests/
├── unit/                  # Unit tests (GTest)
│   ├── common/
│   ├── config/
│   ├── error/
│   ├── io/
│   └── statistics/
├── integration/           # Integration tests
│   └── test_pipeline_integration.cpp
├── e2e/                   # End-to-end tests
│   ├── test_cli.sh        # Shell E2E
│   └── test_advanced_cli.py  # Python E2E
└── utils/                 # Test utilities library
    ├── test_helpers.h
    └── fixture_loader.h
```

---

## Running Tests

```bash
# All tests
./scripts/core/test

# Unit tests only
./scripts/core/test --type unit

# Integration tests only
./scripts/core/test --type integration

# Filter specific tests
./scripts/core/test --filter "*timer*"

# Repeat execution
./scripts/core/test --repeat 5

# Verbose output
./scripts/core/test --verbose

# Coverage report
./scripts/core/test --coverage
```

---

## Unit Tests

Using Google Test framework, define test cases via `TEST` or `TEST_F` macros.

### Example Structure

```cpp
class FastqReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary FASTQ file
    }
    void TearDown() override {
        // Cleanup resources
    }
};

TEST_F(FastqReaderTest, ReadBasic) {
    // Verify reader correctly parses records
    EXPECT_EQ(records[0].id, "read1");
}
```

### CMake Integration

`tests/unit/CMakeLists.txt` provides the `add_unit_test` function, automatically linking GTest, `fq_lib`, and `test_utils`.

---

## Test Utilities Library

`tests/utils/` provides two core utility classes, compiled into `test_utils` static library:

### FixtureLoader

```cpp
// Load predefined test data
auto content = FixtureLoader::loadTextFile("sample.fastq");
auto lines = FixtureLoader::loadLines("expected_output.txt");
```

### TestHelpers

```cpp
// Create temporary files/directories
auto tmpFile = TestHelpers::createTempFile(content, ".fastq");
auto tmpDir = TestHelpers::createTempDir();

// Generate synthetic data
auto records = TestHelpers::generateFastQRecords(1000, 150);
auto dna = TestHelpers::generateRandomDNA(150);

// File comparison
bool same = TestHelpers::compareFiles(file1, file2);

// Cleanup
TestHelpers::cleanup();
```

---

## End-to-End Tests

### Shell E2E (`tests/e2e/test_cli.sh`)

Directly calls `FastQTools` executable, verifying:

- `--help` outputs available commands
- `filter --help` / `stat --help` display options
- `--quiet` suppresses banner
- `filter` / `stat` commands work correctly
- Exit codes are correct

Uses `mktemp -d` + `trap` to ensure temporary resources are automatically cleaned up.

### Python E2E (`tests/e2e/test_advanced_cli.py`)

More complex scenario testing, using Python subprocess calls.

---

## Coverage

```bash
# Generate coverage report
./scripts/core/test --coverage
```

Produces lcov HTML report, focus on:

- Core module coverage
- Uncovered branches and exception paths
- Excludes `tests/`, `build/`, `build-*` directories

---

## Writing New Tests Guide

1. **Use utility library**: Create temporary files via `TestHelpers`, avoid manual management
2. **Resource cleanup**: Release resources in `TearDown()` or `cleanup()`
3. **Independence**: Each test uses independent temporary directory, avoid cross-test side effects
4. **Boundary cases**: Focus on testing empty input, large files, illegal formats, etc.
5. **Performance testing**: Run in Release mode, use `Timer` class for timing

---

## Common Test Failure Troubleshooting

| Symptom | Cause | Solution |
|---------|-------|----------|
| Fixture file not found | Incorrect working directory | Use `FixtureLoader::getFixturePath` for smart lookup |
| Random failures on CI | Concurrent race | Ensure each test uses independent temporary directory |
| `find_package(GTest)` failure | Dependencies not installed | Run `conan install config/dependencies/ --build=missing` |
| Coverage tool missing | lcov not installed | `sudo apt install lcov` |
