# API Overview

FastQTools provides a clean C++ public API, ready for library-level integration into other projects.

---

## Entry Header

```cpp
#include <fqtools/fq.h>
```

`fq.h` aggregates all public interface headers, including:

| Module | Header | Description |
|--------|--------|-------------|
| IO | `<fqtools/io/...>` | FASTQ read/write (FastqReader, FastqWriter, FastqRecord, FastqBatch) |
| Processing | `<fqtools/processing/...>` | Processing pipeline, Predicate, Mutator |
| Statistics | `<fqtools/statistics/...>` | Statistics computation interface |
| Core | `<fqtools/core/core.h>` | Sequence utility functions |
| Config | `<fqtools/config/config.h>` | Configuration management |
| Error | `<fqtools/error/error.h>` | Exception handling framework |
| Logging | `<fqtools/logging.h>` | Logging initialization and level control |
| Common | `<fqtools/common/common.h>` | Timer, IDGenerator, and other utilities |

---

## Module Architecture

```
fq.h (aggregate entry)
  ├── io/          → FastqReader / FastqWriter / FastqRecord / FastqBatch
  ├── processing/  → ProcessingPipelineInterface / Predicate / Mutator
  ├── statistics/  → StatisticCalculatorInterface
  ├── core/        → SequenceUtils
  ├── config/      → Configuration
  ├── error/       → FastQException hierarchy
  ├── logging/     → init / setLevel
  └── common/      → Timer / IDGenerator / Logger
```

---

## Quick Examples

### Statistical Analysis

```cpp
#include <fqtools/fq.h>

fq::statistic::StatisticOptions options;
options.inputFastqPath = "input.fastq.gz";
options.outputStatPath = "output.stat.txt";

auto calculator = fq::statistic::createStatisticCalculator(options);
calculator->run();
```

### Filtering

```cpp
#include <fqtools/fq.h>

auto pipeline = fq::processing::createProcessingPipeline();
pipeline->setInputPath("input.fq.gz");
pipeline->setOutputPath("filtered.fq.gz");

fq::processing::ProcessingConfig config;
config.batchSize = 10000;
config.threadCount = 4;
pipeline->setProcessingConfig(config);

// Add filter predicates
pipeline->addReadPredicate(
    std::make_unique<fq::processing::MinQualityPredicate>(20.0, 33));
pipeline->addReadPredicate(
    std::make_unique<fq::processing::MinLengthPredicate>(50));

// Add trimmer
pipeline->addReadMutator(
    std::make_unique<fq::processing::QualityTrimmer>(
        20.0, 50, fq::processing::QualityTrimmer::TrimMode::Both, 33));

auto stats = pipeline->run();
std::cout << stats.toString() << std::endl;
```

### CMake Integration

```cmake
find_package(FastQTools CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE fq_common)
```

---

## Detailed Documentation

- [IO Module](io.md) — FastqReader / FastqWriter / FastqRecord / FastqBatch
- [Processing Pipeline](processing.md) — Pipeline / Predicate / Mutator
- [Statistics](statistics.md) — StatisticCalculator
- [Core Utilities](core.md) — SequenceUtils / Configuration / Error / Logging
