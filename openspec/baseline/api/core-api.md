# API Specification: Core Interfaces

> **Status**: Active
> **Last Updated**: 2026-04-28
> **Related**: [Product Spec](../product/fastq-processing.md), [Core Architecture](../architecture/0001-core-architecture.md)

## Overview

FastQTools exposes its maintained public C++ surface from `include/fqtools/`. The umbrella header is:

```cpp
#include <fqtools/fq.h>
```

The maintained API goal is **truthful stability with additive evolution**. Only interfaces present in public headers are part of this baseline, and new capabilities should extend the existing `stat` / `filter` workflows instead of creating parallel command families.

## Public Header Map

| Area | Entry points |
| --- | --- |
| Common / config / errors | `fqtools/common/common.h`, `fqtools/config/config.h`, `fqtools/error/error.h` |
| I/O | `fqtools/io/fastq_io.h`, `fqtools/io/fastq_reader.h`, `fqtools/io/fastq_writer.h` |
| Processing | `fqtools/processing/processing_pipeline_interface.h`, `fqtools/processing/read_predicate_interface.h`, `fqtools/processing/read_mutator_interface.h`, `fqtools/processing/predicates.h`, `fqtools/processing/mutators.h` |
| Statistics | `fqtools/statistics/statistic_calculator.h`, `fqtools/statistics/statistic_calculator_interface.h`, `fqtools/statistics/statistic_interface.h` |

## I/O Module (`fq::io`)

### `FastqRecord`

Zero-copy view into a record owned by a `FastqBatch`.

```cpp
namespace fq::io {

struct FastqRecord {
    std::string_view id;
    std::string_view comment;
    std::string_view seq;
    std::string_view qual;
    std::string_view plus;

    [[nodiscard]] auto empty() const -> bool;
    [[nodiscard]] auto length() const -> size_t;
    [[nodiscard]] auto validateLengths() const -> bool;
};

}  // namespace fq::io
```

**Lifecycle rule**: `FastqRecord` does not own memory. All views become invalid when the underlying `FastqBatch` is cleared or destroyed.

### `FastqBatch`

Reusable batch container that owns contiguous buffer storage and a record list.

```cpp
namespace fq::io {

class FastqBatch {
public:
    explicit FastqBatch(size_t capacityBytes = 4 * 1024 * 1024);
    FastqBatch(size_t capacityBytes, size_t expectedRecords);

    void clear();

    [[nodiscard]] auto begin() const;
    [[nodiscard]] auto end() const;
    [[nodiscard]] auto begin();
    [[nodiscard]] auto end();

    [[nodiscard]] auto size() const -> size_t;
    [[nodiscard]] auto empty() const -> bool;

    [[nodiscard]] auto buffer() -> std::vector<char>&;
    [[nodiscard]] auto records() -> std::vector<FastqRecord>&;
    [[nodiscard]] auto buffer() const -> const std::vector<char>&;

    auto moveRemainderToStart(size_t validEndPos) -> size_t;
};

}  // namespace fq::io
```

### `FastqReaderOptions` and `FastqReader`

```cpp
namespace fq::io {

struct FastqReaderOptions {
    size_t readChunkBytes = 1 * 1024 * 1024;
    size_t zlibBufferBytes = 128 * 1024;
    size_t maxBufferBytes = 0;
};

class FastqReader {
public:
    explicit FastqReader(const std::string& path);
    FastqReader(const std::string& path, const FastqReaderOptions& options);
    ~FastqReader();

    FastqReader(const FastqReader&) = delete;
    FastqReader& operator=(const FastqReader&) = delete;
    FastqReader(FastqReader&&) noexcept;
    FastqReader& operator=(FastqReader&&) noexcept;

    [[nodiscard]] auto nextBatch(FastqBatch& batch) -> bool;
    [[nodiscard]] auto nextBatch(FastqBatch& batch, size_t maxRecords) -> bool;
    [[nodiscard]] auto isOpen() const -> bool;
};

}  // namespace fq::io
```

**Supported compression path**: gzip by filename convention (`.gz`).

### `FastqWriterCompressionMode`, `FastqWriterOptions`, and `FastqWriter`

```cpp
namespace fq::io {

enum class FastqWriterCompressionMode : std::uint8_t {
    Auto,
    Gzip,
    None,
};

struct FastqWriterOptions {
    size_t zlibBufferBytes = 128 * 1024;
    size_t outputBufferBytes = 128 * 1024;
    FastqWriterCompressionMode compression = FastqWriterCompressionMode::Auto;
};

class FastqWriter {
public:
    explicit FastqWriter(const std::string& path);
    FastqWriter(const std::string& path, const FastqWriterOptions& options);
    ~FastqWriter();

    FastqWriter(const FastqWriter&) = delete;
    FastqWriter& operator=(const FastqWriter&) = delete;
    FastqWriter(FastqWriter&&) noexcept;
    FastqWriter& operator=(FastqWriter&&) noexcept;

    void write(const FastqBatch& batch);
    void write(const FastqRecord& record);

    [[nodiscard]] auto isOpen() const -> bool;
    [[nodiscard]] auto totalUncompressedBytes() const -> std::uint64_t;
};

}  // namespace fq::io
```

## Processing Module (`fq::processing`)

### Runtime policy enums

```cpp
namespace fq::processing {

enum class ExecutionBackend : std::uint8_t {
    OneTbb,
};

enum class MemoryResourcePolicy : std::uint8_t {
    ObjectPool,
};

}  // namespace fq::processing
```

### Low-level extension interfaces

```cpp
namespace fq::processing {

class ReadPredicateInterface {
public:
    virtual ~ReadPredicateInterface() = default;
    virtual auto evaluate(const fq::io::FastqRecord& read) const -> bool = 0;
};

class ReadMutatorInterface {
public:
    virtual ~ReadMutatorInterface() = default;
    virtual void process(fq::io::FastqRecord& read) = 0;
};

}  // namespace fq::processing
```

### Built-in predicates

```cpp
namespace fq::processing {

class MinQualityPredicate : public ReadPredicateInterface {
public:
    explicit MinQualityPredicate(double minQuality, int qualityEncoding = 33);
};

class MinLengthPredicate : public ReadPredicateInterface {
public:
    explicit MinLengthPredicate(size_t minLength);
};

class MaxLengthPredicate : public ReadPredicateInterface {
public:
    explicit MaxLengthPredicate(size_t maxLength);
};

class MaxNRatioPredicate : public ReadPredicateInterface {
public:
    explicit MaxNRatioPredicate(double maxNRatio);
};

}  // namespace fq::processing
```

### Built-in mutators

```cpp
namespace fq::processing {

class QualityTrimmer : public ReadMutatorInterface {
public:
    enum class TrimMode { Both, FivePrime, ThreePrime };

    QualityTrimmer(double qualityThreshold,
                   size_t minLength = 1,
                   TrimMode mode = TrimMode::Both,
                   int qualityEncoding = 33);
};

class LengthTrimmer : public ReadMutatorInterface {
public:
    enum class TrimStrategy { FixedLength, MaxLength, FromStart, FromEnd };

    LengthTrimmer(size_t targetLength, TrimStrategy strategy = TrimStrategy::FixedLength);
};

class AdapterTrimmer : public ReadMutatorInterface {
public:
    AdapterTrimmer(const std::vector<std::string>& adapterSequences,
                   size_t minOverlap = 3,
                   size_t maxMismatches = 1);
};

class PolyTailTrimmer : public ReadMutatorInterface {
public:
    enum class TailKind { PolyG, PolyX };

    PolyTailTrimmer(TailKind kind, size_t minRunLength = 10);
};

}  // namespace fq::processing
```

### Processing pipeline

```cpp
namespace fq::processing {

struct ProcessingStatistics {
    uint64_t totalReads = 0;
    uint64_t passedReads = 0;
    uint64_t filteredReads = 0;
    uint64_t modifiedReads = 0;
    uint64_t errorReads = 0;
    uint64_t inputBytes = 0;
    uint64_t outputBytes = 0;
    uint64_t elapsedMs = 0;
    double processingTimeMs = 0.0;
    double throughputMbps = 0.0;
    bool allocationTelemetryEnabled = false;
    MemoryResourcePolicy memoryResourcePolicy = MemoryResourcePolicy::ObjectPool;
    size_t resolvedMaxInFlightBatches = 0;

    [[nodiscard]] auto getPassRate() const -> double;
    [[nodiscard]] auto getFilterRate() const -> double;
    [[nodiscard]] auto toString() const -> std::string;
};

struct ProcessingConfig {
    size_t batchSize = 10000;
    size_t threadCount = 1;
    ExecutionBackend executionBackend = ExecutionBackend::OneTbb;
    MemoryResourcePolicy memoryResourcePolicy = MemoryResourcePolicy::ObjectPool;
    bool allocationTelemetryEnabled = false;
    size_t readChunkBytes = 1 * 1024 * 1024;
    size_t zlibBufferBytes = 128 * 1024;
    size_t writerBufferBytes = 128 * 1024;
    size_t batchCapacityBytes = 4 * 1024 * 1024;
    size_t memoryLimitBytes = 0;
    size_t maxInFlightBatches = 0;
};

class ProcessingPipelineInterface {
public:
    virtual ~ProcessingPipelineInterface() = default;

    virtual void setInputPath(const std::string& inputPath) = 0;
    virtual void setOutputPath(const std::string& outputPath) = 0;
    virtual void setProcessingConfig(const ProcessingConfig& config) = 0;
    virtual void addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) = 0;
    virtual void addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) = 0;
    virtual auto run() -> ProcessingStatistics = 0;
};

auto createProcessingPipeline() -> std::unique_ptr<ProcessingPipelineInterface>;

}  // namespace fq::processing
```

## Statistics Module (`fq::statistic`)

### High-level statistics entry point

```cpp
namespace fq::statistic {

struct StatisticOptions {
    std::string inputFastqPath;
    std::string outputStatPath;
    std::string signatureReportPath;
    uint32_t batchSize = 50000;
    size_t signatureKmerSize = 15;
    size_t maxReportedSignatures = 20;
    size_t duplicateEstimateSampleModulo = 1024;
    uint32_t threadCount = 4;
    fq::processing::ExecutionBackend executionBackend = fq::processing::ExecutionBackend::OneTbb;
    fq::processing::MemoryResourcePolicy memoryResourcePolicy =
        fq::processing::MemoryResourcePolicy::ObjectPool;
    bool allocationTelemetryEnabled = false;
    size_t readChunkBytes = 1 * 1024 * 1024;
    size_t zlibBufferBytes = 128 * 1024;
    size_t batchCapacityBytes = 4 * 1024 * 1024;
    size_t memoryLimitBytes = 0;
    size_t maxInFlightBatches = 0;
    int qualityEncoding = 33;
};

class StatisticCalculatorInterface {
public:
    virtual ~StatisticCalculatorInterface() = default;
    virtual void run() = 0;
};

auto createStatisticCalculator(const StatisticOptions& options)
    -> std::unique_ptr<StatisticCalculatorInterface>;

}  // namespace fq::statistic
```

### Low-level statistic extension point

```cpp
namespace fq::statistic {

struct FqStatisticResult;  // forward-declared in the public interface

class StatisticInterface {
public:
    using Batch = fq::io::FastqBatch;
    using Result = FqStatisticResult;

    virtual ~StatisticInterface() = default;
    virtual auto calculateStats(const Batch& batch) -> Result = 0;
};

using IStatistic = StatisticInterface;

}  // namespace fq::statistic
```

**API note**: the maintained public workflow for statistics is `StatisticOptions + createStatisticCalculator(...)->run()`. Optional signature output remains additive through `signatureReportPath`; the default text report is still the primary contract.

## Configuration Module (`fq::config`)

```cpp
namespace fq::config {

using ConfigValue = std::variant<bool, std::int64_t, double, std::string>;

class Configuration {
public:
    void loadFromFile(const std::filesystem::path& configFile);
    void loadFromArgs(int argc, char* argv[]);
    void loadFromEnv();

    template <typename T>
    [[nodiscard]] auto get(std::string_view key) const -> T;

    template <typename T>
    [[nodiscard]] auto getOr(std::string_view key, T defaultValue) const -> T;

    template <typename T>
    void set(std::string_view key, T value);

    [[nodiscard]] auto hasKey(std::string_view key) const -> bool;
    [[nodiscard]] auto empty() const -> bool;
    [[nodiscard]] auto size() const -> size_t;
    [[nodiscard]] auto keys() const -> std::vector<std::string>;
    void clear();
    void validate() const;
    void printConfig(std::ostream& out) const;
    void setFromString(const std::string& key, const std::string& value);
};

auto globalConfig() -> Configuration&;

template <typename T>
auto getConfig(std::string_view key) -> T;

template <typename T>
auto getConfigOr(std::string_view key, T defaultValue) -> T;

template <typename T>
void setConfig(std::string_view key, T value);

}  // namespace fq::config
```

## Error Handling

Public code uses the exception hierarchy in `fqtools/error/error.h`. Configuration access failures surface as `fq::error::ConfigurationError`; I/O and format failures are surfaced by the I/O and CLI layers.

## Stability Notes

1. Public integration must include headers from `include/fqtools/`.
2. Types or helpers only declared under `src/` are implementation details, even if referenced by public interfaces internally.
3. Additive changes should prefer extending `ProcessingConfig`, `ProcessingStatistics`, or `StatisticOptions` instead of inventing separate command-specific configuration surfaces.

## Related Specifications

- [Product Specification](../product/fastq-processing.md)
- [Schema Specification](../schemas/schema.md)
- [Core Architecture](../architecture/0001-core-architecture.md)
