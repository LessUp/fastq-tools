---
hide:
  - navigation
  - toc
---

# FastQTools

<div style="text-align: center; margin: 2rem 0;">
<p style="font-size: 1.4rem; color: #555;">
Modern FASTQ File Processing Toolkit — High-Performance Bioinformatics QC Tool
</p>
</div>

---

## Key Features

<div class="grid cards" markdown>

-   :material-chart-bar:{ .lg .middle } **Statistical Analysis (`stat`)**

    ---

    FASTQ file statistics: base composition, quality distribution, GC content, and more

    ```bash
    FastQTools stat -i input.fastq.gz -o output.stat.txt
    ```

-   :material-filter:{ .lg .middle } **Filtering (`filter`)**

    ---

    Read filtering and trimming with quality, length, N-ratio, and other criteria

    ```bash
    FastQTools filter -i input.fq.gz -o filtered.fq.gz \
        --min-quality 20 --min-length 50
    ```

</div>

## Technical Highlights

| Feature | Description |
|---------|-------------|
| **High Performance** | Parallel pipeline processing based on Intel TBB `parallel_pipeline` |
| **Modern C++** | C++23 standard + modern CMake build system |
| **Modular Design** | Clean interface-implementation separation, library-level integration ready |
| **Zero-Copy I/O** | `FastqRecord` uses `string_view` for efficient memory access |
| **Comprehensive QC** | Sanitizers, Valgrind, fuzz testing, and coverage — fully covered |

## Quick Start

```bash
# One-command build
./scripts/core/build

# Show help
./build/clang-release/FastQTools --help

# Statistical analysis
FastQTools stat -i input.fastq.gz -o output.stat.txt

# Filtering
FastQTools filter -i input.fq.gz -o filtered.fq.gz --min-quality 20 --min-length 50
```

[:octicons-arrow-right-24: Getting Started Guide](guide/getting-started.md){ .md-button .md-button--primary }
[:octicons-book-24: API Reference](api/overview.md){ .md-button }

## Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| **Intel oneTBB** | 2022.3.0 | Parallel pipeline core |
| **zlib-ng** | 2.3.2 | gzip compression/decompression |
| **libdeflate** | 1.25 | High-performance deflate |
| **cxxopts** | 3.1.1 | Command-line argument parsing |
| **spdlog** | 1.17.0 | Logging framework |
| **fmt** | 12.1.0 | Formatting library |
| **nlohmann_json** | 3.11.3 | JSON processing |

## Project Structure

```
FastQTools/
├── include/fqtools/   # Public API headers
├── src/               # Source implementation
├── tests/             # Tests (unit / integration / e2e)
├── config/            # Build config (Conan profiles, sanitizers, valgrind, etc.)
├── scripts/           # Build & utility scripts (core / lib / tools)
├── docker/            # Docker config (dev / prod / deploy)
├── tools/             # Dev tools (benchmark / fuzz / data)
├── cmake/modules/     # Custom CMake modules
├── docs/              # Documentation
├── changelog/         # Change records
└── examples/          # Usage examples
```

---

<div style="text-align: center; margin-top: 2rem; color: #888; font-size: 0.9rem;">
MIT License · Copyright &copy; 2025-2026 LessUp
</div>
