# Processing Pipeline API

Namespace: `fq::processing`

---

## ProcessingPipelineInterface

Abstract interface for processing pipeline, hiding concrete implementation (factory pattern).

### Factory Creation

```cpp
auto pipeline = fq::processing::createProcessingPipeline();
```

### Interface Methods

```cpp
class ProcessingPipelineInterface {
public:
    virtual void setInputPath(const std::string& path) = 0;
    virtual void setOutputPath(const std::string& path) = 0;
    virtual void setProcessingConfig(const ProcessingConfig& config) = 0;
    virtual void addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) = 0;
    virtual void addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) = 0;
    virtual auto run() -> ProcessingStats = 0;
};
```

---

## ProcessingConfig

Pipeline configuration parameters.

| Parameter | Type | Description |
|-----------|------|-------------|
| `batchSize` | `size_t` | Number of reads per batch |
| `threadCount` | `size_t` | Number of parallel threads |
| `readChunkBytes` | `size_t` | Read chunk size |
| `zlibBufferBytes` | `size_t` | zlib buffer |
| `writerBufferBytes` | `size_t` | Writer buffer |
| `batchCapacityBytes` | `size_t` | Batch memory limit |
| `memoryLimitBytes` | `size_t` | Total memory limit |
| `maxInFlightBatches` | `size_t` | Max concurrent batches |

---

## ProcessingStats

Processing result statistics.

| Field | Type | Description |
|-------|------|-------------|
| `totalReads` | `uint64_t` | Total input reads |
| `passedReads` | `uint64_t` | Reads passed filter |
| `filteredReads` | `uint64_t` | Reads filtered out |
| `errorReads` | `uint64_t` | Error reads |
| `inputBytes` | `uint64_t` | Input bytes |
| `outputBytes` | `uint64_t` | Output bytes |
| `elapsedMs` | `uint64_t` | Total time (milliseconds) |
| `throughputMbps` | `double` | Throughput (MB/s) |

```cpp
auto getPassRate() const -> double;
auto getFilterRate() const -> double;
auto toString() const -> std::string;
```

---

## Parallel Processing Mechanism

When `threadCount > 1`, uses `tbb::parallel_pipeline`:

```
Source (serial_in_order) → Processing (parallel) → Sink (serial_in_order)
    Read FastqBatch            Parallel filter/trim         Ordered write results
```

Built-in backpressure mechanism, controls memory usage via `maxInFlightBatches`.

---

## ReadPredicateInterface — Filter Predicate

```cpp
class ReadPredicateInterface {
public:
    virtual auto evaluate(const fq::io::FastqRecord& read) const -> bool = 0;
};
```

### Built-in Implementations

| Class | Description |
|-------|-------------|
| `MinQualityPredicate` | Minimum average quality filter |
| `MinLengthPredicate` | Minimum read length filter |
| `MaxLengthPredicate` | Maximum read length filter |
| `MaxNRatioPredicate` | Maximum N base ratio filter |

```cpp
pipeline->addReadPredicate(
    std::make_unique<MinQualityPredicate>(20.0, 33));
pipeline->addReadPredicate(
    std::make_unique<MinLengthPredicate>(50));
```

---

## ReadMutatorInterface — Read Mutator

```cpp
class ReadMutatorInterface {
public:
    virtual void process(fq::io::FastqRecord& read) = 0;
};
```

### Built-in Implementations

| Class | Description |
|-------|-------------|
| `QualityTrimmer` | Quality trim (Both/FivePrime/ThreePrime modes) |
| `LengthTrimmer` | Length trim (FixedLength/MaxLength/FromStart/FromEnd) |
| `AdapterTrimmer` | Adapter trim |

```cpp
pipeline->addReadMutator(
    std::make_unique<QualityTrimmer>(
        20.0, 50,
        QualityTrimmer::TrimMode::Both, 33));
```

---

## Complete Example

```cpp
#include <fqtools/fq.h>

auto pipeline = fq::processing::createProcessingPipeline();
pipeline->setInputPath("input.fastq");
pipeline->setOutputPath("output.fastq");

fq::processing::ProcessingConfig config;
config.batchSize = 10000;
config.threadCount = 4;
pipeline->setProcessingConfig(config);

pipeline->addReadPredicate(
    std::make_unique<fq::processing::MinQualityPredicate>(20.0, 33));
pipeline->addReadPredicate(
    std::make_unique<fq::processing::MinLengthPredicate>(50));
pipeline->addReadMutator(
    std::make_unique<fq::processing::QualityTrimmer>(
        20.0, 50, fq::processing::QualityTrimmer::TrimMode::Both, 33));

auto stats = pipeline->run();
std::cout << stats.toString() << std::endl;
```
