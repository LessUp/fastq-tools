# 统计分析 API

命名空间：`fq::statistic`

---

## StatisticCalculatorInterface

统计计算任务的抽象接口，通过工厂函数创建实例。

### 工厂创建

```cpp
fq::statistic::StatisticOptions options;
options.inputFastqPath = "input.fastq.gz";
options.outputStatPath = "output.stat.txt";
options.signatureReportPath = "output.signatures.tsv";
options.threadCount = 4;

auto calculator = fq::statistic::createStatisticCalculator(options);
calculator->run();
```

### 接口定义

```cpp
class StatisticCalculatorInterface {
public:
    virtual ~StatisticCalculatorInterface() = default;
    virtual void run() = 0;
};
```

---

## StatisticOptions

统计任务配置。

| 字段 | 类型 | 说明 |
|------|------|------|
| `inputFastqPath` | `std::string` | 输入 FASTQ 文件路径 |
| `outputStatPath` | `std::string` | 输出统计文件路径 |
| `signatureReportPath` | `std::string` | 可选的 signature sidecar 路径 |
| `batchSize` | `uint32_t` | 每批次处理的记录数 |
| `signatureKmerSize` | `size_t` | sidecar 头部 k-mer 长度 |
| `maxReportedSignatures` | `size_t` | sidecar 最多输出的 signature 数 |
| `duplicateEstimateSampleModulo` | `size_t` | duplicate estimate 的采样模数 |
| `threadCount` | `uint32_t` | 线程数 |
| `executionBackend` | `ExecutionBackend` | 当前支持 `OneTbb` |
| `memoryResourcePolicy` | `MemoryResourcePolicy` | 当前支持 `ObjectPool` |
| `allocationTelemetryEnabled` | `bool` | 是否在文本报告中输出内存遥测头部 |
| `readChunkBytes` | `size_t` | Reader chunk 大小 |
| `zlibBufferBytes` | `size_t` | zlib 内部缓冲区大小 |
| `batchCapacityBytes` | `size_t` | 单批缓冲区容量 |
| `memoryLimitBytes` | `size_t` | 用于推导 in-flight 上限的内存预算 |
| `maxInFlightBatches` | `size_t` | 显式 in-flight 批次上限（`0` 为自动） |
| `qualityEncoding` | `int` | 质量编码偏移量（通常为 `33`） |

---

## StatisticInterface

统计计算的底层抽象接口，支持自定义统计逻辑扩展。

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

公共头文件只前向声明 `FqStatisticResult`；下表描述的是当前实现暴露给统计器/聚合流程的主要结果语义，而不是稳定 ABI 布局。

| 字段 | 类型 | 说明 |
|------|------|------|
| `readCount` | `uint64_t` | 读段总数 |
| `totalBases` | `uint64_t` | 碱基总数 |
| `maxReadLength` | `uint32_t` | 最大读长 |
| `duplicateSampledReads` | `uint64_t` | 采样命中的 duplicate reads 数 |
| `posQualityDist` | `vector<uint64_t>` | 扁平化位置质量分布 |
| `posBaseDist` | `vector<uint64_t>` | 扁平化位置碱基分布 |
| `headKmerCounts` | `map<string, uint64_t>` | bounded head-kmer signature 计数 |

支持 `operator+=` 合并多个批次的统计结果。

---

## 并行处理架构

统计分析使用 TBB 并行管道：

```
输入文件 → FastqReader → [Source] → [Processing] → [Aggregation] → 输出文件
                          串行读取    并行统计计算    串行合并结果
```

1. **Source（serial_in_order）**：串行读取 FastqBatch
2. **Processing（parallel）**：并行计算每个 batch 的统计信息
3. **Aggregation（serial_in_order）**：合并所有统计结果

---

## 输出格式

统计结果默认输出文本报告，并可选输出一个 TSV sidecar，包含：

- 读段总数、最大读长、总碱基数
- 碱基组成（A/T/C/G/N 比例）
- GC 含量
- 位置质量分布（Q20/Q30 百分比）
- 逐位置碱基计数、平均质量与估计错误率
- duplicate estimate（基于采样）
- bounded head-kmer signatures（通过 `--signature-report` 启用）

当启用 `--signature-report` 时，sidecar 采用以下 TSV 结构：

```text
metric	key	count
summary	total_reads	<count>
summary	duplicate_estimate	<count>
head_kmer	<kmer>	<count>
```
