# API 概览

FastQTools 的公共 C++ API 以 `include/fqtools/fq.h` 为 Façade 入口，聚合所有对外头文件。

## 头文件结构

```cpp
#include <fqtools/fq.h>  // 一站式入口，聚合以下全部
```

| 模块 | 头文件 | 用途 |
|------|--------|------|
| common | `fqtools/common/common.h` | 共享工具 |
| config | `fqtools/config/config.h` | 配置 |
| error | `fqtools/error/error.h` | 异常基类与宏 |
| logging | `fqtools/logging.h` | 日志 |
| io | `fqtools/io/fastq_io.h` | `FastqRecord`/`FastqBatch` 核心数据结构 |
| io | `fqtools/io/fastq_reader.h` / `fastq_writer.h` | 读取器/写入器 |
| io | `fqtools/io/interfaces.h` | `IReader`/`IWriter` 抽象接口 |
| processing | `fqtools/processing/processing_options.h` | `ProcessingOptions`/`ProcessingProfile` |
| processing | `fqtools/processing/interfaces.h` | `ReadPredicateInterface`/`ReadMutatorInterface` |
| processing | `fqtools/processing/predicates.h` / `mutators.h` | 内置谓词/修改器 |
| processing | `fqtools/processing/processing_pipeline_interface.h` | 管道接口 |
| statistics | `fqtools/statistics/interfaces.h` | `StatisticOptions`/`StatisticCalculatorInterface` |
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

## 统计工作流（受支持路径）

```cpp
#include <fqtools/fq.h>

fq::statistic::StatisticOptions options;
options.inputFastqPath = "sample.fastq.gz";
options.outputStatPath = "sample.stats.txt";
options.processing.batchSize = 50000;
options.processing.threadCount = 8;

auto calculator = fq::statistic::createStatisticCalculator(options);
calculator->run();
```

## 过滤工作流

```cpp
#include <fqtools/fq.h>

auto pipeline = fq::processing::createProcessingPipeline();
pipeline->setInputPath("sample.fastq.gz");
pipeline->setOutputPath("sample.filtered.fastq.gz");

// 谓词：保留满足条件的读段
pipeline->addReadPredicate(std::make_unique<fq::processing::MinQualityPredicate>(20.0));

// 修改器：变换读段
pipeline->addReadMutator(std::make_unique<fq::processing::QualityTrimmer>(20, "both"));

fq::processing::ProcessingOptions opts;
opts.batchSize = 50000;
opts.threadCount = 8;
pipeline->setProcessingOptions(opts);

auto stats = pipeline->run();
```

## 依赖注入

`ProcessingPipeline::setReader` / `setWriter` 接受 `unique_ptr<IReader>` / `unique_ptr<IWriter>`，测试可注入 mock。接口定义在 `fqtools/io/interfaces.h`。

## 错误处理

库内部抛 `fq::error::FastQException` 子类（`IOError`/`FormatError`/`ConfigurationError`）。宏 `FQ_THROW_IO_ERROR`/`FQ_THROW_FORMAT_ERROR`/`FQ_THROW_CONFIG_ERROR` 统一抛点。调用方在边界捕获。
