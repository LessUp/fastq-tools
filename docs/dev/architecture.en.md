# Architecture

## Core Design Principles

- **Performance First**: Maximize processing throughput and resource utilization
- **Modern C++**: Full adoption of C++23 standard features
- **High Cohesion, Low Coupling**: Clear module boundaries and interface design

## Technology Stack

| Area | Technology | Rationale |
|------|-----------|-----------|
| **Language** | C++23 | Modern features, excellent performance |
| **Build** | CMake | C++ project standard |
| **Dependencies** | Conan | Automated third-party library management |
| **Concurrency** | Intel TBB | High-performance parallel computing |
| **CLI** | cxxopts | Lightweight command-line parsing |
| **Logging** | spdlog | High-performance logging library |
| **Testing** | GoogleTest | Powerful testing framework |

## Architecture Layers

```
User / Caller
    ↓
CLI Layer (src/cli/)
    ↓
Application Command Layer (src/cli/commands/)
    ↓
Core Library Layer (include/fqtools/)
    ↓
Implementation Layer (src/pipeline/, src/modules/)
```

### 1. CLI Layer
- Entry point: `src/cli/main.cpp`
- Responsibilities: Argument parsing, command dispatch, logging initialization
- Uses: cxxopts + spdlog

### 2. Application Command Layer
- Interface: `CommandInterface` (`command_interface.h`)
- Implementations: `StatCommand`, `FilterCommand`
- Responsibilities: Argument validation, business logic invocation

### 3. Core Library Layer
- Public API: `include/fqtools/`
- Key interfaces:
  - `processing_pipeline.h`: `ProcessingPipelineInterface`
  - `statistic_calculator.h`: `StatisticCalculatorInterface`
  - `fq.h`: Aggregate header file

### 4. Implementation Layer
- Pipeline & processing: `src/processing/`
- Statistics: `src/statistics/`
- IO: `src/io/`

## Concurrency Model

Uses `tbb::parallel_pipeline` for high-performance parallel processing:

- **source**: Serial FASTQ record reading
- **processing**: Parallel filtering and transformation
- **sink**: Serial result writing

Built-in backpressure mechanism automatically balances I/O and computation speed.

## Build System

Modern CMake target decomposition:

```cmake
# Base modules
fq_common
fq_error
fq_config
fq_core
fq_modern_io
 
# Core functionality
fq_pipeline
 
# Application layer
fq_cli
```

Dependencies flow bottom-up, ensuring modular design.
