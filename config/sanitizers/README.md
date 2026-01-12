# Sanitizer Configuration

This directory contains configuration files for compiler sanitizers.

## Available Sanitizers

| Sanitizer | Purpose | Compiler Support |
|-----------|---------|------------------|
| ASan | Memory errors (buffer overflow, use-after-free) | GCC, Clang |
| TSan | Thread data races | GCC, Clang |
| UBSan | Undefined behavior | GCC, Clang |
| MSan | Uninitialized memory reads | Clang only |

## Usage

### Build with Sanitizer

```bash
# AddressSanitizer
./scripts/core/build --sanitizer asan --dev

# ThreadSanitizer
./scripts/core/build --sanitizer tsan --dev

# UndefinedBehaviorSanitizer
./scripts/core/build --sanitizer ubsan --dev

# MemorySanitizer (Clang only)
./scripts/core/build --sanitizer msan --dev -c clang
```

### Configure Sanitizer Options

Set environment variables before running:

```bash
# ASan options
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=0:print_stats=1"

# TSan options
export TSAN_OPTIONS="halt_on_error=0:second_deadlock_stack=1"

# UBSan options
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=0"

# MSan options
export MSAN_OPTIONS="halt_on_error=0:print_stats=1"
```

### Run Tests with Sanitizer

```bash
# Build with ASan and run tests
./scripts/core/build --sanitizer asan --dev
./scripts/core/test -b build-clang-debug
```

## Important Notes

1. **Mutually Exclusive**: ASan, TSan, and MSan cannot be used together
2. **UBSan**: Can be combined with ASan or TSan
3. **MSan**: Requires Clang and all dependencies compiled with MSan
4. **Performance**: Sanitizers add significant overhead (2-10x slower)
5. **Debug Builds**: Recommended for better stack traces

## Suppression Files

Create suppression files to ignore known false positives:

- `tsan.supp` - ThreadSanitizer suppressions
- `lsan.supp` - LeakSanitizer suppressions (used with ASan)

Example suppression:
```
# tsan.supp
race:third_party_library
deadlock:known_safe_pattern
```
