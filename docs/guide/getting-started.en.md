# Getting Started

This guide helps you set up FastQTools and run your first command in minimal time.

---

## System Requirements

### Runtime (Minimum)

- Ubuntu 20.04+ or Debian 11+
- 1GB RAM / 100MB disk

### Development (Recommended)

- Ubuntu 22.04+ or Debian 12+
- 4GB RAM / 2GB disk
- Multi-core CPU (speeds up parallel builds)

---

## Install Dependencies

```bash
# Development environment (compilers, build tools, debug tools)
./scripts/core/install-deps

# Runtime only (production deployment)
./scripts/core/install-deps --runtime

# Show help
./scripts/core/install-deps --help
```

**Development dependencies include:**

- GCC 15 / Clang 21 compilers
- CMake, Ninja build system
- Conan 2.x package manager
- GDB, Valgrind debugging tools
- lcov coverage tool

**Runtime dependencies include:**

- libtbb12 (Intel TBB parallel library)
- zlib, bzip2, xz compression libraries
- libdeflate (high-performance decompression)

---

## Build

```bash
# Default build (Clang + Release)
./scripts/core/build

# Development mode (Debug + verbose output)
./scripts/core/build --dev

# Specify compiler and build type
./scripts/core/build --compiler gcc --type Debug

# Show all options
./scripts/core/build --help
```

After a successful build, the executable is at `build/clang-release/FastQTools` (or the corresponding build directory).

---

## Verify Installation

```bash
# Show help
./build/clang-release/FastQTools --help

# Run tests
./scripts/core/test
```

---

## First Example

### 1. Prepare Test Data

```bash
# Generate test FASTQ file
python3 tools/data/gen_fastq.py -o test_data.fastq --reads 5000 --min-len 80 --max-len 150
```

### 2. Statistical Analysis

```bash
FastQTools stat -i test_data.fastq -o stats.txt --threads 4
```

Output includes: total reads, length distribution, base composition (A/T/C/G/N), GC content, Q20/Q30 percentages, etc.

### 3. Quality Filtering

```bash
FastQTools filter -i test_data.fastq -o filtered.fastq \
  --min-quality 20 \
  --min-length 50 \
  --max-n-ratio 0.1 \
  --trim-quality 20 \
  --trim-mode both
```

---

## Troubleshooting

### Dependency installation fails

- Ensure system is Ubuntu/Debian-based
- Manually update package list: `sudo apt-get update`
- Check network connectivity

### Build fails

- Confirm Conan is installed: `conan --version`
- Confirm CMake is available: `cmake --version`
- Confirm compiler is available: `clang++ --version` or `g++ --version`

### Executable not found

- Check build directory: `ls build/clang-release/FastQTools`
- Confirm build script completed successfully (exit code 0)

### Missing shared libraries at runtime

```bash
# Check missing libraries
ldd build/clang-release/FastQTools

# Install runtime dependencies
./scripts/core/install-deps --runtime
```

---

## Next Steps

- [CLI Reference](cli-reference.md) — Full command-line usage
- [Configuration](configuration.md) — Environment variables and config files
- [Deployment](deployment.md) — Docker and production deployment
- [Build Guide](../dev/build.md) — Advanced build options
