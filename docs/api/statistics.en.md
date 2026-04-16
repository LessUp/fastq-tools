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
| `threadCount` | `size_t` | Number of threads |
| `batchSize` | `size_t` | Batch processing size |

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

Statistics result data structure.

| Field | Type | Description |
|-------|------|-------------|
| `readCount` | `uint64_t` | Total read count |
| `totalBases` | `uint64_t` | Total base count |
| `maxReadLength` | `uint32_t` | Maximum read length |
| `posQualityDist` | `vector<vector<uint64_t>>` | Position quality distribution |
| `posBaseDist` | `vector<vector<uint64_t>>` | Position base distribution |

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

Statistics results support JSON and text format output, containing:

- Total reads, valid reads
- Sequence length distribution (min/max/average)
- Base composition (A/T/C/G/N ratio)
- GC content
- Position quality distribution (Q20/Q30 percentage)
