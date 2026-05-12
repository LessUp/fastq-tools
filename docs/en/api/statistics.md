# Statistics API

Namespace: `fq::statistic`

---

## StatisticCalculatorInterface

Abstract interface for statistical calculation tasks, create instances via factory function.

### Factory Creation

```cpp
fq::statistic::StatisticOptions options;
options.inputFastqPath = "input.fastq.gz";
options.outputStatPath = "output.stat.txt";
options.signatureReportPath = "output.signatures.tsv";
options.threadCount = 4;

auto calculator = fq::statistic::createStatisticCalculator(options);
calculator->run();
```

### Interface Definition

```cpp
class StatisticCalculatorInterface {
public:
    virtual ~StatisticCalculatorInterface() = default;
    virtual void run() = 0;
};
```

---

## StatisticOptions

Statistics task configuration.

| Field | Type | Description |
|-------|------|-------------|
| `inputFastqPath` | `std::string` | Input FASTQ file path |
| `outputStatPath` | `std::string` | Output statistics file path |
| `signatureReportPath` | `std::string` | Optional signature sidecar path |
| `batchSize` | `uint32_t` | Number of records processed per batch |
| `signatureKmerSize` | `size_t` | Head-kmer size used by the sidecar |
| `maxReportedSignatures` | `size_t` | Maximum number of signature rows to emit |
| `duplicateEstimateSampleModulo` | `size_t` | Sampling modulo used for duplicate estimation |
| `threadCount` | `uint32_t` | Number of threads |
| `executionBackend` | `ExecutionBackend` | Currently supports `OneTbb` |
| `memoryResourcePolicy` | `MemoryResourcePolicy` | Currently supports `ObjectPool` |
| `allocationTelemetryEnabled` | `bool` | Emits memory telemetry headers in the text report |
| `readChunkBytes` | `size_t` | Reader chunk size |
| `zlibBufferBytes` | `size_t` | zlib internal buffer size |
| `batchCapacityBytes` | `size_t` | Per-batch buffer capacity |
| `memoryLimitBytes` | `size_t` | Memory budget used to resolve in-flight batches |
| `maxInFlightBatches` | `size_t` | Explicit in-flight batch cap (`0` means auto) |
| `qualityEncoding` | `int` | Quality encoding offset (typically `33`) |

---

## StatisticInterface

Low-level abstract interface for statistical calculation, supports custom statistics logic extension.

```cpp
class StatisticInterface {
public:
    using Batch = fq::io::FastqBatch;
    using Result = FqStatisticResult;

    virtual ~StatisticInterface() = default;
    virtual auto calculateStats(const Batch& batch) -> Result = 0;
};
```

---

## FqStatisticResult

The public header only forward-declares `FqStatisticResult`; the table below describes the current result semantics used by workers and aggregators rather than a stable ABI layout.

| Field | Type | Description |
|-------|------|-------------|
| `readCount` | `uint64_t` | Total read count |
| `totalBases` | `uint64_t` | Total base count |
| `maxReadLength` | `uint32_t` | Maximum read length |
| `duplicateSampledReads` | `uint64_t` | Number of sampled duplicate reads |
| `posQualityDist` | `vector<uint64_t>` | Flattened position-quality distribution |
| `posBaseDist` | `vector<uint64_t>` | Flattened position-base distribution |
| `headKmerCounts` | `map<string, uint64_t>` | Bounded head-kmer signature counts |

Supports `operator+=` to merge statistics results from multiple batches.

---

## Parallel Processing Architecture

Statistical analysis uses TBB parallel pipeline:

```
Input file → FastqReader → [Source] → [Processing] → [Aggregation] → Output file
                          Serial read  Parallel calc    Serial merge
```

1. **Source (serial_in_order)**: Serially read FastqBatch
2. **Processing (parallel)**: Parallelly calculate statistics for each batch
3. **Aggregation (serial_in_order)**: Merge all statistics results

---

## Output Format

Statistics results write a text report by default and may optionally emit a TSV sidecar, containing:

- Total reads, max read length, and total bases
- Base composition (A/T/C/G/N ratio)
- GC content
- Position quality distribution (Q20/Q30 percentage)
- Per-position base counts, average quality, and estimated error rate
- Duplicate estimate (sampling-based)
- Bounded head-kmer signatures (enabled via `--signature-report`)

When `--signature-report` is enabled, the sidecar uses this TSV shape:

```text
metric	key	count
summary	total_reads	<count>
summary	duplicate_estimate	<count>
head_kmer	<kmer>	<count>
```
