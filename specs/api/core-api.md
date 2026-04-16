# API Specification: Core Interfaces

> **Status**: Active  
> **Last Updated**: 2026-04-17  
> **Related**: [Product Spec](../product/fastq-processing.md), [RFC-0001](../rfc/0001-core-architecture.md)

## Overview

This document defines the core C++ API interfaces for FastQTools. All public interfaces are located in `include/fqtools/` and follow stable API conventions.

## I/O Module (`fq::io`)

### FastqRecord

Lightweight view into FASTQ record data.

```cpp
namespace fq::io {

struct FastqRecord {
    std::string_view id;
    std::string_view comment;
    std::string_view sequence;
    std::string_view quality;

    [[nodiscard]] auto isValid() const -> bool;
    [[nodiscard]] auto length() const -> size_t;
    [[nodiscard]] auto averageQuality() const -> double;
};

}  // namespace fq::io
```

**Lifecycle**: `FastqRecord` instances are views into `FastqBatch` memory. The batch must outlive all records.

### FastqBatch

Contiguous memory block holding multiple FASTQ records.

```cpp
namespace fq::io {

class FastqBatch {
public:
    explicit FastqBatch(size_t capacity);

    [[nodiscard]] auto records() const -> std::vector<FastqRecord>;
    [[nodiscard]] auto count() const -> size_t;
    [[nodiscard]] auto capacity() const -> size_t;

    void addRecord(std::string_view id, std::string_view seq, std::string_view qual);
    void clear();

    [[nodiscard]] auto memoryUsage() const -> size_t;
};

}  // namespace fq::io
```

**Invariants**:
- `count() <= capacity()` always holds
- All records reference valid offsets into `buffer`
- Memory is contiguous (enables efficient parsing)

### FastqReader

Reads FASTQ files with automatic compression detection.

```cpp
namespace fq::io {

class FastqReader {
public:
    explicit FastqReader(std::string_view filePath);
    ~FastqReader();

    // Read next batch
    auto readBatch(size_t maxRecords = 10000) -> std::unique_ptr<FastqBatch>;

    // Iteration support
    [[nodiscard]] auto hasNext() const -> bool;
    [[nodiscard]] auto getTotalReads() const -> size_t;

    // Disable copy
    FastqReader(const FastqReader&) = delete;
    auto operator=(const FastqReader&) = delete;
};

}  // namespace fq::io
```

**Error Handling**:
- File not found → throws `fq::error::IOError`
- Invalid format → throws `fq::error::FormatError` with line number
- Compression error → throws `fq::error::IOError` with details

### FastqWriter

Writes FASTQ files with compression support.

```cpp
namespace fq::io {

class FastqWriter {
public:
    FastqWriter(std::string_view filePath, CompressionType compression = CompressionType::None);
    ~FastqWriter();

    void writeBatch(const FastqBatch& batch);
    void writeRecord(const FastqRecord& record);
    void close();

    [[nodiscard]] auto getWrittenReads() const -> size_t;

    // Disable copy
    FastqWriter(const FastqWriter&) = delete;
    auto operator=(const FastqWriter&) = delete;
};

}  // namespace fq::io
```

## Processing Module (`fq::processing`)

### Pipeline

TBB-based parallel processing pipeline.

```cpp
namespace fq::processing {

class Pipeline {
public:
    Pipeline();
    ~Pipeline();

    // Add processing stage
    template<typename Func>
    void addStage(Func&& stage);

    // Execute pipeline
    auto execute(std::unique_ptr<FastqBatch> batch) -> std::unique_ptr<FastqBatch>;

    // Configuration
    void setThreadCount(size_t threads);
    void setBatchSize(size_t size);

    [[nodiscard]] auto getThreadCount() const -> size_t;
};

}  // namespace fq::processing
```

### Predicate

Filtering conditions for read validation.

```cpp
namespace fq::processing {

class Predicate {
public:
    virtual ~Predicate() = default;
    [[nodiscard]] virtual auto passes(const FastqRecord& record) const -> bool = 0;
};

// Built-in predicates
class QualityPredicate : public Predicate {
public:
    explicit QualityPredicate(double minQuality);
    [[nodiscard]] auto passes(const FastqRecord& record) const -> bool override;
};

class LengthPredicate : public Predicate {
public:
    explicit LengthPredicate(size_t minLength, size_t maxLength = SIZE_MAX);
    [[nodiscard]] auto passes(const FastqRecord& record) const -> bool override;
};

class NBasesPredicate : public Predicate {
public:
    explicit NBasesPredicate(double maxNRatio);
    [[nodiscard]] auto passes(const FastqRecord& record) const -> bool override;
};

}  // namespace fq::processing
```

### Mutator

Read modification operations.

```cpp
namespace fq::processing {

class Mutator {
public:
    virtual ~Mutator() = default;
    virtual void apply(FastqRecord& record) const = 0;
};

// Quality trimming
class QualityTrimmer : public Mutator {
public:
    enum class Mode { BothEnds, FivePrime, ThreePrime };

    QualityTrimmer(double qualityThreshold, Mode mode = Mode::BothEnds);
    void apply(FastqRecord& record) const override;
};

// Length filtering
class LengthFilter : public Mutator {
public:
    explicit LengthFilter(size_t minLength);
    void apply(FastqRecord& record) const override;
};

}  // namespace fq::processing
```

## Statistics Module (`fq::statistic`)

### StatisticCalculator

Calculates various FASTQ statistics.

```cpp
namespace fq::statistic {

struct StatisticsResult {
    size_t totalReads;
    size_t totalBases;
    size_t q20Count;
    size_t q30Count;
    double gcContent;
    double atContent;
    double nContent;
    std::map<size_t, size_t> lengthDistribution;
    std::array<double, 4> baseComposition;  // A, T, C, G
};

class StatisticCalculator {
public:
    StatisticCalculator();

    void processBatch(const FastqBatch& batch);
    [[nodiscard]] auto getResult() const -> StatisticsResult;
    void reset();

    // Convenience function
    static auto calculateFromFile(std::string_view filePath) -> StatisticsResult;
};

}  // namespace fq::statistic
```

## Error Handling

All errors use exception hierarchy:

```cpp
namespace fq::error {

class FastQException : public std::exception {
public:
    explicit FastQException(std::string message);
    [[nodiscard]] auto what() const noexcept -> const char* override;
};

class IOError : public FastQException {
public:
    using FastQException::FastQException;
};

class FormatError : public FastQException {
public:
    using FastQException::FastQException;
};

class ConfigurationError : public FastQException {
public:
    using FastQException::FastQException;
};

}  // namespace fq::error
```

**Convenience Macros**:
```cpp
#define FQ_THROW_IO_ERROR(msg) throw fq::error::IOError(msg)
#define FQ_THROW_FORMAT_ERROR(msg) throw fq::error::FormatError(msg)
#define FQ_THROW_CONFIG_ERROR(msg) throw fq::error::ConfigurationError(msg)
```

## Logging

Uses spdlog-based logging:

```cpp
namespace fq::logging {

void trace(std::string_view fmt, auto&&... args);
void debug(std::string_view fmt, auto&&... args);
void info(std::string_view fmt, auto&&... args);
void warn(std::string_view fmt, auto&&... args);
void error(std::string_view fmt, auto&&... args);
void critical(std::string_view fmt, auto&&... args);

}  // namespace fq::logging
```

**Usage**: `fq::logging::info("Processing batch: {} records", count);`

## Factory Functions

### Pipeline Factory

```cpp
namespace fq::processing {

auto createProcessingPipeline(const PipelineConfig& config) 
    -> std::unique_ptr<Pipeline>;

}  // namespace fq::processing
```

### Statistics Factory

```cpp
namespace fq::statistic {

auto createStatisticCalculator(StatisticType type) 
    -> std::unique_ptr<StatisticCalculator>;

}  // namespace fq::statistic
```

## Conventions

### Naming

| Element | Style | Example |
|---------|-------|---------|
| Classes | PascalCase | `FastqReader`, `QualityPredicate` |
| Functions | camelCase | `readBatch()`, `calculateFromFile()` |
| Variables | camelCase | `totalReads`, `filePath` |
| Private members | camelCase_ | `config_`, `pipeline_` |
| Constants | kCamelCase | `kDefaultBatchSize` |
| Namespaces | lower_case | `fq::io`, `fq::processing` |

### Header Files

- Public API: `#include <fqtools/module/file.h>`
- Use `#pragma once` for header guards
- Avoid `using namespace` in headers

### Return Types

- Use trailing return types: `auto foo() -> int`
- Use `[[nodiscard]]` for queries, getters, state checks
- Use `std::string_view` for read-only string parameters

## Related Documents

- [Product Specification](../product/fastq-processing.md)
- [RFC-0001: Core Architecture](../rfc/0001-core-architecture.md)
- [Coding Standards](../../docs/dev/coding-standards.md)
