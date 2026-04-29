# 处理流水线 API

命名空间：`fq::processing`

---

## ProcessingPipelineInterface

处理流水线的抽象接口，隐藏具体实现（工厂模式）。

### 工厂创建

```cpp
auto pipeline = fq::processing::createProcessingPipeline();
```

### 接口方法

```cpp
class ProcessingPipelineInterface {
public:
    virtual void setInputPath(const std::string& path) = 0;
    virtual void setOutputPath(const std::string& path) = 0;
    virtual void setProcessingConfig(const ProcessingConfig& config) = 0;
    virtual void addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) = 0;
    virtual void addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) = 0;
    virtual auto run() -> ProcessingStatistics = 0;
};
```

---

## ProcessingConfig

流水线配置参数。

| 参数 | 类型 | 说明 |
|------|------|------|
| `batchSize` | `size_t` | 每批 reads 数量 |
| `threadCount` | `size_t` | 并行线程数 |
| `executionBackend` | `ExecutionBackend` | 当前支持 `OneTbb` |
| `memoryResourcePolicy` | `MemoryResourcePolicy` | 当前支持 `ObjectPool` |
| `allocationTelemetryEnabled` | `bool` | 是否启用内存遥测 |
| `readChunkBytes` | `size_t` | 读取块大小 |
| `zlibBufferBytes` | `size_t` | zlib 缓冲区 |
| `writerBufferBytes` | `size_t` | 写入缓冲区 |
| `batchCapacityBytes` | `size_t` | 批次内存限制 |
| `memoryLimitBytes` | `size_t` | 总内存限制 |
| `maxInFlightBatches` | `size_t` | 并发批次数 |

---

## ProcessingStatistics

处理结果统计。

| 字段 | 类型 | 说明 |
|------|------|------|
| `totalReads` | `uint64_t` | 输入读段总数 |
| `passedReads` | `uint64_t` | 通过过滤的读段数 |
| `filteredReads` | `uint64_t` | 被过滤的读段数 |
| `modifiedReads` | `uint64_t` | 被修剪或修改的读段数 |
| `errorReads` | `uint64_t` | 错误读段数 |
| `inputBytes` | `uint64_t` | 输入字节数 |
| `outputBytes` | `uint64_t` | 输出字节数 |
| `elapsedMs` | `uint64_t` | 总耗时（毫秒） |
| `processingTimeMs` | `double` | 处理耗时（毫秒，浮点表示） |
| `throughputMbps` | `double` | 吞吐量（MB/s） |
| `allocationTelemetryEnabled` | `bool` | 是否启用内存遥测 |
| `memoryResourcePolicy` | `MemoryResourcePolicy` | 解析后的内存策略 |
| `resolvedMaxInFlightBatches` | `size_t` | 本次运行解析后的 in-flight 上限 |

```cpp
auto getPassRate() const -> double;
auto getFilterRate() const -> double;
auto toString() const -> std::string;
```

---

## 并行处理机制

当 `threadCount > 1` 时，使用 `tbb::parallel_pipeline`：

```
Source（serial_in_order）→ Processing（parallel）→ Sink（serial_in_order）
    读取 FastqBatch          并行执行过滤/修剪         有序写入结果
```

内置背压机制，通过 `maxInFlightBatches` 控制内存使用。

---

## ReadPredicateInterface — 过滤谓词

```cpp
class ReadPredicateInterface {
public:
    virtual auto evaluate(const fq::io::FastqRecord& read) const -> bool = 0;
};
```

### 内置实现

| 类 | 说明 |
|----|------|
| `MinQualityPredicate` | 最小平均质量过滤 |
| `MinLengthPredicate` | 最小读长过滤 |
| `MaxLengthPredicate` | 最大读长过滤 |
| `MaxNRatioPredicate` | 最大 N 碱基比例过滤 |

```cpp
pipeline->addReadPredicate(
    std::make_unique<MinQualityPredicate>(20.0, 33));
pipeline->addReadPredicate(
    std::make_unique<MinLengthPredicate>(50));
```

---

## ReadMutatorInterface — 读段修饰器

```cpp
class ReadMutatorInterface {
public:
    virtual void process(fq::io::FastqRecord& read) = 0;
};
```

### 内置实现

| 类 | 说明 |
|----|------|
| `QualityTrimmer` | 质量修剪（支持 Both/FivePrime/ThreePrime 模式） |
| `LengthTrimmer` | 长度修剪（FixedLength/MaxLength/FromStart/FromEnd） |
| `AdapterTrimmer` | 接头修剪 |
| `PolyTailTrimmer` | polyG / bounded polyX 尾巴修剪 |

```cpp
pipeline->addReadMutator(
    std::make_unique<QualityTrimmer>(
        20.0, 50,
        QualityTrimmer::TrimMode::Both, 33));
```

---

## 完整示例

```cpp
#include <fqtools/fq.h>

auto pipeline = fq::processing::createProcessingPipeline();
pipeline->setInputPath("input.fastq");
pipeline->setOutputPath("output.fastq");

fq::processing::ProcessingConfig config;
config.batchSize = 10000;
config.threadCount = 4;
config.executionBackend = fq::processing::ExecutionBackend::OneTbb;
pipeline->setProcessingConfig(config);

pipeline->addReadPredicate(
    std::make_unique<fq::processing::MinQualityPredicate>(20.0, 33));
pipeline->addReadPredicate(
    std::make_unique<fq::processing::MinLengthPredicate>(50));
pipeline->addReadMutator(
    std::make_unique<fq::processing::QualityTrimmer>(
        20.0, 50, fq::processing::QualityTrimmer::TrimMode::Both, 33));
pipeline->addReadMutator(
    std::make_unique<fq::processing::AdapterTrimmer>(
        std::vector<std::string>{"AGATCGGAAGAGC"}, 6, 1));

auto stats = pipeline->run();
std::cout << stats.toString() << "\n";
```
