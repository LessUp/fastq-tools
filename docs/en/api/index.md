# API Reference

Public C++ API reference documentation for FastQTools.

## Modules

- [Overview](overview.md) - API design principles and conventions
- [IO Module](io.md) - FASTQ file read/write interfaces
- [Processing Pipeline](processing.md) - Data processing pipe interfaces
- [Statistics](statistics.md) - Statistical computation interfaces
- [Core Utilities](core.md) - General utilities and helpers

## Naming Conventions

| Type | Rule | Example |
|------|------|---------|
| Classes/Structs | PascalCase | `FastqBatch`, `StatCommand` |
| Functions/Methods | camelCase | `validateLengths()`, `getPassRate()` |
| Regular Variables | camelCase | `totalReads`, `inputFile` |
| Constants | kCamelCase | `kDefaultBatchSize` |
| Enum Values | PascalCase | `CompressionType::Gzip` |

## Header File Organization

All public headers are located in `include/fqtools/`, organized by module:

```
include/fqtools/
├── io/          # I/O related interfaces
├── processing/  # Processing pipe interfaces
├── statistics/  # Statistics interfaces
├── benchmark/   # Benchmark interfaces
├── config/      # Configuration interfaces
├── error/       # Error handling
└── common/      # Common types
```

## Usage Example

```cpp
#include <fqtools/io/fastq_reader.h>
#include <fqtools/processing/processing_pipeline.h>

// Using the API
auto reader = fqtools::io::FastqReader{"input.fastq"};
auto pipeline = fqtools::processing::ProcessingPipeline{};
```
