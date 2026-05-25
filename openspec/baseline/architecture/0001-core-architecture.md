# Architecture-0001: Core Architecture

> **Status**: Active
> **Created**: 2026-04-17
> **Last Updated**: 2026-05-24
> **Authors**: FastQTools Core Team
> **Related**: [Product Spec](../product/fastq-processing.md)

## Context

FastQTools requires a high-performance, maintainable architecture for processing FASTQ files in bioinformatics workflows. The architecture must support parallel processing, zero-copy I/O, and a narrow maintained surface centered on the retained CLI commands and the documented embeddable API subset.

## Decision

### Architecture Pattern

The system follows a **single maintained execution path** with interface-implementation separation:

```
┌─────────────────────────────────────────────────────────┐
│                    CLI Layer                             │
│       (Retained commands: stat / filter only)           │
├─────────────────────────────────────────────────────────┤
│            Public Library Interfaces                     │
│ (`fqtools/fq.h` + documented API subset for retained     │
│  processing/statistics flows; shared support headers     │
│  may also be consumed by the CLI layer)                  │
├─────────────────────────────────────────────────────────┤
│          Core Implementations in src/                    │
│  ┌──────────┬──────────┬──────────┬────────────┐        │
│  │ I/O      │Processing│Statistics│ Utilities  │        │
│  │ Module   │ Module   │ Module   │ Module     │        │
│  └──────────┴──────────┴──────────┴────────────┘        │
└─────────────────────────────────────────────────────────┘
```

### Key Architectural Decisions

#### 1. Interface-Implementation Separation

- **Public API**: `include/fqtools/fq.h` plus the documented `common/`, `config/`, `error/`, `io/`, and `processing/` header families, along with the supported statistics workflow exposed via `fqtools/statistics/statistic_calculator_interface.h` - stable, versioned embeddable interface
- **Implementation**: `src/` - internal implementation details
- **CLI Layer**: keeps the maintained `stat` / `filter` processing path and the supported statistics calculator workflow on the documented embeddable API subset, while shared support headers such as `fqtools/logging.h` and other installed-but-unsupported statistics headers may still be consumed by the CLI

#### 2. Zero-Copy I/O Model

```cpp
// FastqBatch owns contiguous memory
struct FastqBatch {
    std::vector<char> buffer;  // Raw data
    // ...
};

// FastqRecord views into batch memory
struct FastqRecord {
    std::string_view id;
    std::string_view sequence;
    std::string_view quality;
};
```

**Rationale**: Eliminates string copying during FASTQ parsing, improving performance by 3-5x.

#### 3. TBB Parallel Pipeline

Processing uses `tbb::parallel_pipeline` for data parallelism:

```
Reader (serial) → Transform (parallel) → Writer (serial)
                      ↓
              [Multiple parallel stages]
```

**Rationale**: Maximizes CPU utilization for multi-core systems while maintaining simple programming model.

#### 4. Factory Pattern

Object creation via factory functions:
- `createProcessingPipeline()` → returns pipeline implementation
- `createStatisticCalculator(options)` → returns calculator implementation

**Rationale**: Keeps the CLI and embeddable library on the same public construction path while preserving testability.

#### 5. Command Pattern

CLI subcommands implement `CommandInterface`:
```cpp
class CommandInterface {
public:
    virtual ~CommandInterface() = default;
    virtual auto execute(const ParsedArgs& args) -> int = 0;
};
```

**Rationale**: Keeps dispatch for the retained commands isolated, testable, and aligned with the public library interfaces.

## Technical Stack

| Component | Technology | Version |
|-----------|-----------|---------|
| Language | C++ | 23 |
| Parallel Processing | Intel oneTBB | 2022.3.0 |
| Build System | CMake + Conan | 3.28+ / 2.x |
| Compression | libdeflate + zlib-ng | 1.25 / 2.3.2 |
| Logging | spdlog | 1.17.0 |
| CLI Parsing | cxxopts | 3.1.1 |
| JSON | nlohmann_json | 3.11.3 |

## Namespaces

| Namespace | Purpose |
|-----------|---------|
| `fq::io` | FASTQ I/O (Reader, Writer, FastqRecord, FastqBatch) |
| `fq::processing` | Pipeline, Predicate, Mutator |
| `fq::statistic` | Statistical calculations |
| `fq::cli` | CLI interface |
| `fq::cli::commands` | Subcommand implementations |
| `fq::common` | Common utilities |
| `fq::config` | Configuration management |
| `fq::error` | Error handling |
| `fq::logging` | Logging (based on spdlog) |

## Correctness Properties

### Property 1: Memory Safety

*For any* FASTQ file processing operation, the system SHALL NOT leak memory or access invalid memory.

**Validation**: Valgrind memcheck, AddressSanitizer, CI integration.

### Property 2: Thread Safety

*For any* parallel pipeline execution, the system SHALL NOT have data races.

**Validation**: ThreadSanitizer, deterministic test cases.

### Property 3: Output Determinism

*For any* given input file and configuration, the output SHALL be identical across runs (except for non-deterministic ordering in parallel operations where order is not specified).

**Validation**: End-to-end tests with fixed inputs.

### Property 4: Resource Bounding

*For any* input file size, memory usage SHALL NOT grow unboundedly.

**Validation**: Streaming processing design, memory profiling.

## Error Handling

| Error Type | Handling Strategy |
|-----------|-------------------|
| File not found | Throw `fq::error::IOError` with path |
| Invalid FASTQ format | Throw `fq::error::FormatError` with line number |
| Configuration error | Throw `fq::error::ConfigurationError` |
| Out of memory | Throw `std::bad_alloc`, log error |
| Parallel execution failure | Propagate exception, cleanup resources |

## Testing Strategy

1. **Unit Tests**: Individual module testing in `tests/unit/`
2. **Integration Tests**: Cross-module integration in `tests/integration/`
3. **End-to-End Tests**: CLI testing in `tests/e2e/`
4. **Property-Based Tests**: Universal property validation
5. **Sanitizer Tests**: Runtime error detection (ASan, TSan, UBSan)

## Alternatives Considered

### Alternative 1: OpenMP Parallelism
**Rejected because**: TBB provides more flexible pipeline parallelism, better load balancing, and integrates better with C++ code.

### Alternative 2: Manual Memory Management
**Rejected because**: Smart pointers and string_view provide safety without performance penalty in C++23.

### Alternative 3: Single-threaded Design
**Rejected because**: Modern sequencing data volumes require parallel processing for practical throughput.

## Consequences

### Positive
- Clean separation enables independent module evolution
- Zero-copy design achieves high performance
- TBB pipeline scales with CPU cores
- Factory pattern enables easy testing

### Negative
- TBB adds dependency complexity
- Zero-copy requires careful lifetime management
- Parallel debugging can be challenging

## Related Documents

- [Product Specification](../product/fastq-processing.md)
- [Architecture-0002: Toolchain Policy](0002-toolchain-policy.md)
- [API Specification](../api/core-api.md)
- [Legacy Architecture Guide](../../../docs/dev/architecture.md)
