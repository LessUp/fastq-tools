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
| `threadCount` | `size_t` | 线程数 |
| `batchSize` | `size_t` | 批处理大小 |

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

统计结果数据结构。

| 字段 | 类型 | 说明 |
|------|------|------|
| `readCount` | `uint64_t` | 读段总数 |
| `totalBases` | `uint64_t` | 碱基总数 |
| `maxReadLength` | `uint32_t` | 最大读长 |
| `posQualityDist` | `vector<vector<uint64_t>>` | 位置质量分布 |
| `posBaseDist` | `vector<vector<uint64_t>>` | 位置碱基分布 |

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

统计结果支持 JSON 和文本格式输出，包含：

- 读段总数、有效读段数
- 序列长度分布（最小/最大/平均）
- 碱基组成（A/T/C/G/N 比例）
- GC 含量
- 位置质量分布（Q20/Q30 百分比）
