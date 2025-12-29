# 使用指南

## 快速开始

```bash
# 查看帮助
./build-clang-release/FastQTools --help

# 统计命令帮助
./build-clang-release/FastQTools stat --help

# 过滤命令帮助
./build-clang-release/FastQTools filter --help
```

## stat 命令 - 统计分析

### 基本用法

```bash
# 单端文件分析
FastQTools stat -i input.fastq.gz -o output.stat.txt

# 多线程处理
FastQTools stat -i reads.fq.gz -o analysis.txt -t 8
```

### 输出指标

- 读数统计：总读数、有效读数
- 长度分布：最小/最大/平均长度
- 质量分析：Q20/Q30 碱基百分比
- 碱基组成：A/T/C/G/N 比例
- GC 含量：整体和位置特异性

## filter 命令 - 过滤与修剪

### 基本用法

```bash
# 质量过滤
FastQTools filter -i input.fq.gz -o filtered.fq.gz --min-quality 20 --min-length 50

# 质量修剪
FastQTools filter -i input.fq.gz -o trimmed.fq.gz --trim-quality 20 --trim-mode three
```

### 过滤选项

- `--min-quality <float>`: 最小平均质量阈值
- `--min-length <int>`: 最小读长
- `--max-length <int>`: 最大读长
- `--max-n-ratio <0.0-1.0>`: 最大 N 碱基比例
- `--trim-quality <float>`: 质量修剪阈值
- `--trim-mode <both|five|three>`: 修剪模式

## 全局选项

- `-v, --verbose`: 详细日志
- `-q, --quiet`: 仅错误输出
- `--log-level=LEVEL`: 设置日志级别 (trace|debug|info|warn|error)

## 公共 API 使用

```cpp
#include <fqtools/fq.h>

// 使用统计接口
fq::statistic::StatisticOptions options;
options.inputFastqPath = "input.fastq.gz";
options.outputStatPath = "output.stat.txt";

auto calculator = fq::statistic::createStatisticCalculator(options);
calculator->run();

// 使用处理流水线
fq::processing::ProcessingConfig config;
config.threadCount = 4;

auto pipeline = fq::processing::createProcessingPipeline();
pipeline->setInputPath("input.fq.gz");
pipeline->setOutputPath("filtered.fq.gz");
pipeline->setProcessingConfig(config);

auto stats = pipeline->run();
```
