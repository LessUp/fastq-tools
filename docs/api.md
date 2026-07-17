# API 概览

FastQTools 的公共 C++ API 以 `include/fqtools/fq.h` 为 Façade 入口，聚合所有对外头文件。

## 头文件结构

```cpp
#include <fqtools/fq.h>  // 一站式入口，聚合以下全部
```

| 模块 | 头文件 | 用途 |
|------|--------|------|
| error | `fqtools/error/error.h` | 异常基类与宏 |
| io | `fqtools/io/fastq_io.h` | `FastqRecord`/`FastqBatch` 核心数据结构 |
| io | `fqtools/io/fastq_reader.h` / `fastq_writer.h` | 读取器/写入器 |
| io | `fqtools/io/interfaces.h` | `IReader`/`IWriter` 抽象接口 |
| processing | `fqtools/processing/processing_options.h` | `ProcessingOptions`/`ProcessingProfile` |
| processing | `fqtools/processing/interfaces.h` | `ReadPredicateInterface`/`ReadMutatorInterface` |
| processing | `fqtools/processing/predicates.h` / `mutators.h` | 内置谓词/修改器 |
| processing | `fqtools/processing/processing_pipeline_interface.h` | move-only `Pipeline` |
| statistics | `fqtools/statistics/interfaces.h` | `StatisticOptions`/move-only `Calculator` |
| statistics | `fqtools/statistics/statistics_writer.h` | 统计报告写入 |

## 核心数据结构

### FastqRecord — 零拷贝记录视图

```cpp
struct FastqRecord {
    std::string_view id;       // 序列 ID（不含 @ 前缀）
    std::string_view comment;  // ID 行注释
    std::string_view seq;      // 序列
    std::string_view qual;     // 质量值
    std::string_view plus;     // '+' 行原文
    // 生命周期绑定到所属 FastqBatch，不可逃逸
};
```

### FastqBatch — 批量容器

拥有连续内存，记录视图指向其内部偏移。`clear()` 复用内存不释放。

### I/O adapter 契约

```cpp
class IReader {
public:
    virtual auto nextBatch(
        FastqBatch& batch,
        size_t maxRecords = std::numeric_limits<size_t>::max()) -> bool = 0;
};

class IWriter {
public:
    virtual auto write(const FastqBatch& batch) -> std::uint64_t = 0;
};
```

- Reader 每次调用前应清理并重新填充 `batch`，不得返回超过 `maxRecords` 的记录。
- `false` 只表示 EOF；格式或 I/O 错误应抛出项目异常，不能静默转换为 EOF。
- Writer 返回本批接受的未压缩 FASTQ 序列化字节数，用于吞吐和提交计量；它不表示已经 `fsync`。
- 自定义 adapter 不需要继承或识别 `FastqReader` / `FastqWriter` 具体类型。

## 统计工作流（受支持路径）

```cpp
#include <fqtools/fq.h>

fq::statistics::StatisticOptions options;
options.inputFastqPath = "sample.fastq.gz";
options.outputStatPath = "sample.stats.txt";
options.processing.batchSize = 50000;
options.processing.threadCount = 8;

fq::statistics::Calculator calculator(std::move(options));
calculator.run();
```

## 过滤工作流

```cpp
#include <fqtools/fq.h>

fq::processing::Pipeline pipeline;
pipeline.setInputPath("sample.fastq.gz");
pipeline.setOutputPath("sample.filtered.fastq.gz");

// 谓词：保留满足条件的读段
pipeline.addReadPredicate(std::make_unique<fq::processing::MinQualityPredicate>(20.0));

// 修改器：变换读段
pipeline.addReadMutator(std::make_unique<fq::processing::QualityTrimmer>(20, "both"));

fq::processing::ProcessingOptions opts;
opts.batchSize = 50000;
opts.threadCount = 8;
pipeline.setProcessingOptions(opts);

auto stats = pipeline.run();
```

## 依赖注入

`Pipeline::setReader` / `setWriter` 接受 `unique_ptr<IReader>` / `unique_ptr<IWriter>`，测试可注入 mock。接口定义在 `fqtools/io/interfaces.h`。

注入任一自定义 I/O adapter 时，`Automatic` runtime 会选择 Sequential backend，避免对调用方实现施加隐式线程安全要求。自定义 Reader 在一次 `run()` 中被消费；再次运行前必须调用 `setReader()` 注入新实例。自定义 Writer 会被转换为共享所有权，其 `write()` 返回值必须遵守上述计量契约。

> C++ adapter 兼容性提示：旧的单参数 `IReader::nextBatch` 和返回 `void` 的 `IWriter::write` 实现需要更新；CLI 参数和行为不受影响。

## 错误处理

库内部抛 `fq::error::FastQException` 子类（`IOError`/`FormatError`/`ConfigurationError`）。宏 `FQ_THROW_IO_ERROR`/`FQ_THROW_FORMAT_ERROR`/`FQ_THROW_CONFIG_ERROR` 统一抛点。调用方在边界捕获。
