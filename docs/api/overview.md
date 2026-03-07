# API 概览

FastQTools 提供清晰的 C++ 公共 API，支持作为库集成到其他项目中。

---

## 入口头文件

```cpp
#include <fqtools/fq.h>
```

`fq.h` 聚合了所有公共接口头文件，包括：

| 模块 | 头文件 | 说明 |
|------|--------|------|
| IO | `<fqtools/io/...>` | FASTQ 读写（FastqReader, FastqWriter, FastqRecord, FastqBatch） |
| 处理 | `<fqtools/processing/...>` | 处理流水线、Predicate、Mutator |
| 统计 | `<fqtools/statistics/...>` | 统计计算接口 |
| 核心 | `<fqtools/core/core.h>` | 序列工具函数 |
| 配置 | `<fqtools/config/config.h>` | 配置管理 |
| 错误 | `<fqtools/error/error.h>` | 异常处理框架 |
| 日志 | `<fqtools/logging.h>` | 日志初始化与级别控制 |
| 通用 | `<fqtools/common/common.h>` | Timer、IDGenerator 等工具 |

---

## 模块架构

```
fq.h（聚合入口）
  ├── io/          → FastqReader / FastqWriter / FastqRecord / FastqBatch
  ├── processing/  → ProcessingPipelineInterface / Predicate / Mutator
  ├── statistics/  → StatisticCalculatorInterface
  ├── core/        → SequenceUtils
  ├── config/      → Configuration
  ├── error/       → FastQException 异常体系
  ├── logging/     → init / setLevel
  └── common/      → Timer / IDGenerator / Logger
```

---

## 快速示例

### 统计分析

```cpp
#include <fqtools/fq.h>

fq::statistic::StatisticOptions options;
options.inputFastqPath = "input.fastq.gz";
options.outputStatPath = "output.stat.txt";

auto calculator = fq::statistic::createStatisticCalculator(options);
calculator->run();
```

### 过滤处理

```cpp
#include <fqtools/fq.h>

auto pipeline = fq::processing::createProcessingPipeline();
pipeline->setInputPath("input.fq.gz");
pipeline->setOutputPath("filtered.fq.gz");

fq::processing::ProcessingConfig config;
config.batchSize = 10000;
config.threadCount = 4;
pipeline->setProcessingConfig(config);

// 添加过滤条件
pipeline->addReadPredicate(
    std::make_unique<fq::processing::MinQualityPredicate>(20.0, 33));
pipeline->addReadPredicate(
    std::make_unique<fq::processing::MinLengthPredicate>(50));

// 添加修剪器
pipeline->addReadMutator(
    std::make_unique<fq::processing::QualityTrimmer>(
        20.0, 50, fq::processing::QualityTrimmer::TrimMode::Both, 33));

auto stats = pipeline->run();
std::cout << stats.toString() << std::endl;
```

### CMake 集成

```cmake
find_package(FastQTools REQUIRED)
target_link_libraries(my_app PRIVATE FastQTools::FastQTools)
```

---

## 详细文档

- [IO 模块](io.md) — FastqReader / FastqWriter / FastqRecord / FastqBatch
- [处理流水线](processing.md) — Pipeline / Predicate / Mutator
- [统计分析](statistics.md) — StatisticCalculator
- [核心工具](core.md) — SequenceUtils / Configuration / Error / Logging
