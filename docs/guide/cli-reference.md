# CLI 参考

FastQTools 提供 `stat` 和 `filter` 两个子命令，用于 FASTQ 文件的统计分析和质量过滤。

---

## 基本语法

```bash
FastQTools [全局选项] <子命令> [子命令选项]
```

> 全局选项必须位于子命令之前。

---

## 全局选项

| 选项 | 说明 |
|------|------|
| `-v, --verbose` | 启用调试日志（等价于 `--log-level=debug`） |
| `-q, --quiet` | 仅输出错误（等价于 `--log-level=error`） |
| `--log-level=LEVEL` | 设置日志级别：`trace`, `debug`, `info`, `warn`, `error` |
| `--help` | 显示帮助信息 |

默认日志级别为 `info`。

---

## stat 命令 — 统计分析

对 FASTQ 文件进行全面的质量统计分析。

### 用法

```bash
FastQTools stat -i <input> -o <output> [选项]
```

### 选项

| 选项 | 说明 |
|------|------|
| `-i, --input <path>` | 输入 FASTQ 文件（支持 `.gz` 压缩） |
| `-o, --output <path>` | 输出统计文件 |
| `-t, --threads <N>` | 线程数（默认自动检测） |
| `--batch-size <N>` | 每批处理的 reads 数量 |

### 输出指标

- **读段统计**：总读段数、有效读段数
- **长度分布**：最小/最大/平均长度
- **质量分析**：Q20/Q30 碱基百分比
- **碱基组成**：A/T/C/G/N 比例
- **GC 含量**：整体和位置特异性

### 示例

```bash
# 基本统计
FastQTools stat -i reads.fq.gz -o analysis.txt

# 多线程处理
FastQTools stat -i reads.fq.gz -o analysis.txt -t 8

# 调试模式
FastQTools -v stat -i reads.fq.gz -o analysis.txt
```

---

## filter 命令 — 过滤与修剪

对原始测序数据进行清洗、过滤和质量修剪。

### 用法

```bash
FastQTools filter -i <input> -o <output> [选项]
```

### 过滤选项

| 选项 | 说明 |
|------|------|
| `-i, --input <path>` | 输入 FASTQ 文件（支持 `.gz`） |
| `-o, --output <path>` | 输出 FASTQ 文件（`.gz` 后缀自动压缩） |
| `-t, --threads <N>` | 线程数 |
| `--min-quality <float>` | 最小平均质量阈值 |
| `--min-length <int>` | 最小读长 |
| `--max-length <int>` | 最大读长 |
| `--max-n-ratio <0.0-1.0>` | 最大 N 碱基比例 |

### 修剪选项

| 选项 | 说明 |
|------|------|
| `--trim-quality <float>` | 质量修剪阈值 |
| `--trim-mode <mode>` | 修剪模式：`both`（两端）、`five`（5' 端）、`three`（3' 端） |

### 示例

```bash
# 质量过滤
FastQTools filter -i input.fq.gz -o filtered.fq.gz \
  --min-quality 20 --min-length 50

# 质量修剪（从 3' 端移除低质量碱基）
FastQTools filter -i input.fq.gz -o trimmed.fq.gz \
  --trim-quality 20 --trim-mode three

# 组合过滤 + 修剪
FastQTools filter -i input.fq.gz -o clean.fq.gz \
  --min-quality 20 --min-length 50 --max-n-ratio 0.1 \
  --trim-quality 20 --trim-mode both

# 静默模式
FastQTools -q filter -i input.fq.gz -o filtered.fq.gz --min-quality 20
```

---

## 退出码

| 退出码 | 含义 |
|--------|------|
| `0` | 成功 |
| 非 `0` | 错误（未知子命令、参数错误、运行时异常等） |

---

## 公共 API 集成

FastQTools 也可作为 C++ 库使用：

```cpp
#include <fqtools/fq.h>

// 统计分析
fq::statistic::StatisticOptions options;
options.inputFastqPath = "input.fastq.gz";
options.outputStatPath = "output.stat.txt";
auto calculator = fq::statistic::createStatisticCalculator(options);
calculator->run();

// 过滤处理
auto pipeline = fq::processing::createProcessingPipeline();
pipeline->setInputPath("input.fq.gz");
pipeline->setOutputPath("filtered.fq.gz");

fq::processing::ProcessingConfig config;
config.threadCount = 4;
pipeline->setProcessingConfig(config);

auto stats = pipeline->run();
```

详细 API 文档见 [API 参考](../api/overview.md)。
