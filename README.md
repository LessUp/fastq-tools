# FastQTools

[![CI](https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/fastq-tools/actions/workflows/ci.yml)
[![Deploy Docs](https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml/badge.svg)](https://github.com/LessUp/fastq-tools/actions/workflows/pages.yml)
[![Documentation](https://img.shields.io/badge/docs-GitHub%20Pages-blue?logo=github)](https://lessup.github.io/fastq-tools/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
![C++](https://img.shields.io/badge/C++-23-blue.svg)

English | [简体中文](README.zh-CN.md)

> **Online Docs**: [https://lessup.github.io/fastq-tools/](https://lessup.github.io/fastq-tools/)

A modern C++ high-performance FASTQ file processing toolkit for bioinformatics QC, filtering, and statistical analysis.

## Quick Start

```bash
# One-click build
./scripts/core/build

# View help
./build/release/bin/fqtools --help

# Quality control
./build/release/bin/fqtools qc -i input.fastq -o report.json

# Filtering
./build/release/bin/fqtools filter -i input.fastq -o output.fastq -q 20 -l 50
```

## Features

- **Quality Control** — Per-base/per-read quality statistics, GC content, length distribution
- **Read Filtering** — Quality threshold, length range, N-ratio, complexity filtering
- **Statistics** — Comprehensive FASTQ metrics with JSON/HTML reports
- **High Performance** — Multi-threaded I/O, SIMD-accelerated quality parsing
- **Modern C++** — C++23, CMake 3.20+, Conan 2.x dependencies

## Tech Stack

- C++23, CMake 3.20+, Conan 2.x
- Google Test, Google Benchmark
- zlib (compressed FASTQ support)

## Build

```bash
# Install dependencies
conan install . --build=missing -of=build

# Build
cmake --preset release
cmake --build build/build/Release -j$(nproc)

# Run tests
ctest --test-dir build/build/Release
```

## Project Structure

```text
fastq-tools/
├── src/                # Source code
│   ├── core/           # Core library (parser, filter, stats)
│   ├── cli/            # CLI entry point
│   └── io/             # I/O layer (buffered, compressed)
├── include/            # Public headers
├── tests/              # Unit & integration tests
├── benchmarks/         # Performance benchmarks
├── scripts/            # Build & utility scripts
└── docs/               # Documentation
```

## License

MIT License
