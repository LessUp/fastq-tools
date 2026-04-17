# API 参考

FastQTools 的公共 C++ API 参考文档。

## 模块

- [概览](overview.md) - API 设计原则和约定
- [IO 模块](io.md) - FASTQ 文件读写接口
- [处理流水线](processing.md) - 数据处理管道接口
- [统计分析](statistics.md) - 统计计算接口
- [核心工具](core.md) - 通用工具和辅助类

## 命名约定

| 类型 | 规则 | 示例 |
|------|------|------|
| 类/结构体 | PascalCase | `FastqBatch`, `StatCommand` |
| 函数/方法 | camelCase | `validateLengths()`, `getPassRate()` |
| 普通变量 | camelCase | `totalReads`, `inputFile` |
| 常量 | kCamelCase | `kDefaultBatchSize` |
| 枚举值 | PascalCase | `CompressionType::Gzip` |

## 头文件组织

所有公共头文件位于 `include/fqtools/` 目录下，按模块分类：

```
include/fqtools/
├── io/          # I/O 相关接口
├── processing/  # 处理管道接口
├── statistics/  # 统计接口
├── benchmark/   # 基准测试接口
├── config/      # 配置接口
├── error/       # 错误处理
└── common/      # 通用类型
```

## 使用示例

```cpp
#include <fqtools/io/fastq_reader.h>
#include <fqtools/processing/processing_pipeline.h>

// 使用 API
auto reader = fqtools::io::FastqReader{"input.fastq"};
auto pipeline = fqtools::processing::ProcessingPipeline{};
```
