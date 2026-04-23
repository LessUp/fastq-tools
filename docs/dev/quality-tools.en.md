# Code Quality Tools Guide

This document introduces the code quality tools used in the FastQTools project, including installation, usage, and output interpretation.

## Overview

| Tool Category | Tool | Purpose | Execution Timing |
|--------------|------|---------|------------------|
| Memory Check | ASan | Memory error detection | Every significant change |
| Memory Check | MSan | Uninitialized memory | Manual |
| Thread Check | TSan | Data race detection | Manual |
| Undefined Behavior | UBSan | UB detection | Manual |
| Deep Analysis | Valgrind | Memory leaks, cache analysis | Weekly |
| Static Analysis | clang-tidy | Code quality | Every significant change |
| Static Analysis | Cppcheck | Additional checks | Every significant change |
| Coverage | lcov | Test coverage | As needed |
| Security Testing | libFuzzer | Fuzz testing | Manual |

## Sanitizers

### Installation

Sanitizers are built-in compiler features, no additional installation needed. Recommended to use GCC 15 or modern Clang; local Clang builds can be automatically adapted by scripts to installed version via Conan.

### Usage

```bash
# AddressSanitizer - Memory error detection
./scripts/core/build --sanitizer asan --dev
./scripts/core/test -b build/clang-debug

# ThreadSanitizer - Data race detection
./scripts/core/build --sanitizer tsan --dev
./scripts/core/test -b build/clang-debug

# UndefinedBehaviorSanitizer - Undefined behavior detection
./scripts/core/build --sanitizer ubsan --dev
./scripts/core/test -b build/clang-debug

# MemorySanitizer - Uninitialized memory detection (Clang only)
./scripts/core/build --sanitizer msan --dev -c clang
./scripts/core/test -b build/clang-debug
```

### Configuration Options

Environment variable configuration files are located in `config/sanitizers/`:

```bash
# Use configuration files
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=0:print_stats=1"
export TSAN_OPTIONS="halt_on_error=0:second_deadlock_stack=1"
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=0"
```

### Output Interpretation

**ASan Error Example:**
```
==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x...
READ of size 1 at 0x... thread T0
    #0 0x... in function_name file.cpp:123
    #1 0x... in main main.cpp:45
```

- `heap-buffer-overflow`: Heap buffer overflow
- `stack-buffer-overflow`: Stack buffer overflow
- `use-after-free`: Use after free
- `double-free`: Double free

**TSan Error Example:**
```
WARNING: ThreadSanitizer: data race (pid=12345)
  Write of size 8 at 0x... by thread T1:
    #0 function_name file.cpp:100
  Previous read of size 8 at 0x... by thread T2:
    #0 other_function file.cpp:200
```

## Valgrind

### Installation

```bash
# Ubuntu/Debian
sudo apt-get install valgrind

# macOS
brew install valgrind
```

### Usage

```bash
# Run tests with Valgrind
./scripts/core/test -b build/clang-debug --valgrind

# Manual Memcheck run
./scripts/tools/analysis/valgrind-memcheck

# Cache performance analysis
./scripts/tools/performance/valgrind-cachegrind
```

### Output Interpretation

**Memory Leak Report:**
```
==12345== LEAK SUMMARY:
==12345==    definitely lost: 100 bytes in 1 blocks
==12345==    indirectly lost: 0 bytes in 0 blocks
==12345==    possibly lost: 50 bytes in 2 blocks
==12345==    still reachable: 1,000 bytes in 10 blocks
```

- `definitely lost`: Confirmed leak, must be fixed
- `indirectly lost`: Indirect leak, usually fixed with definitely lost
- `possibly lost`: Possible leak, needs checking
- `still reachable`: Still reachable at program end, usually global objects

### Suppression Rules

Known third-library false positives are configured in `config/valgrind/suppressions.supp`.

## Static Analysis

### clang-tidy

```bash
# Run clang-tidy
./scripts/core/lint tidy

# Auto-fix
./scripts/core/lint tidy-fix
```

Configuration file: `.clang-tidy`

### Cppcheck

```bash
# Install
sudo apt-get install cppcheck

# Run
./scripts/core/lint cppcheck
```

Configuration file: `config/cppcheck/cppcheck.cfg`

### Include-What-You-Use

```bash
# Install
sudo apt-get install iwyu

# Run
./scripts/core/lint iwyu
```

Mapping file: `config/iwyu/mappings.imp`

### Run All Checks

```bash
./scripts/core/lint all
```

## Code Coverage

### Installation

```bash
# Ubuntu/Debian
sudo apt-get install lcov

# Optional: Cobertura XML generation
pip install gcovr
```

### Usage

```bash
# Build and run tests
./scripts/core/build --coverage --dev
./scripts/core/test -b build/clang-debug

# Generate report
./scripts/tools/analysis/coverage-report

# Generate XML report (for CI)
./scripts/tools/analysis/coverage-report --xml

# Check threshold
./scripts/tools/analysis/coverage-report --check-threshold
```

### Output

- HTML report: `coverage_report/html/index.html`
- XML report: `coverage_report/coverage.xml`

### Threshold Configuration

Edit `config/coverage/thresholds.json`:

```json
{
  "line_coverage": { "minimum": 70, "target": 80 },
  "function_coverage": { "minimum": 80, "target": 90 }
}
```

## Fuzz Testing

### Installation

Fuzz testing requires Clang compiler and libFuzzer (built into Clang).

### Usage

```bash
# Build and run
./scripts/tools/analysis/run-fuzzer --build

# Run specific fuzzer
./scripts/tools/analysis/run-fuzzer parser -t 300

# Parallel run
./scripts/tools/analysis/run-fuzzer -j 4 -t 3600
```

### Corpus

Seed files are located in `tools/fuzz/corpus/`. Adding new seed files can improve fuzz testing efficiency.

### Crash Analysis

When fuzzer finds a crash:

1. Crash input is saved in `build/fuzz/fuzz_artifacts/`
2. Reproduce the issue with crash input: `./fuzzers/fastq_parser_fuzzer crash-xxx`
3. After fixing, add crash input to corpus as regression test

## Frequently Asked Questions

### Q: ASan reports "LeakSanitizer: detected memory leaks" but code looks correct?

A: May be known third-party library issues. Check if in suppression file, or add new suppression rule.

### Q: TSan reports false positives?

A: TSan may not fully understand some synchronization primitives. Use `__tsan_acquire` and `__tsan_release` annotations, or add suppression rules.

### Q: Valgrind runs too slow?

A: Valgrind is typically 10-50× slower than normal execution. Use ASan for daily development. Valgrind is suitable for periodic deep checks.

### Q: Coverage report shows 0%?

A: Ensure:
1. Built with `--coverage`
2. Tests were run
3. Tests actually executed the code under test

### Q: Fuzzer didn't find any issues?

A: That's good! But also could be:
1. Run time too short, increase `-t` parameter
2. Corpus not good enough, add more seed files
3. Input length limit too small, increase `-l` parameter

## Reference Links

- [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- [ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html)
- [Valgrind Manual](https://valgrind.org/docs/manual/manual.html)
- [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)
- [libFuzzer](https://llvm.org/docs/LibFuzzer.html)
