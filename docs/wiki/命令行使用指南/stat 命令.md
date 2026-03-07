# stat 命令

<cite>
**本文档引用文件**   
- [stat_command.cpp](file://src/cli/commands/stat_command.cpp)
- [stat_command.h](file://src/cli/commands/stat_command.h)
- [fq_statistic.cpp](file://src/statistics/fq_statistic.cpp)
- [fq_statistic.h](file://src/statistics/fq_statistic.h)
- [fq_statistic_worker.cpp](file://src/statistics/fq_statistic_worker.cpp)
- [fq_statistic_worker.h](file://src/statistics/fq_statistic_worker.h)
- [statistic_calculator_interface.h](file://include/fqtools/statistics/statistic_calculator_interface.h)
- [statistic_interface.h](file://include/fqtools/statistics/statistic_interface.h)
- [basic_stats.sh](file://examples/basic_usage/basic_stats.sh)
- [README.md](file://README.md)
- [usage.md](file://docs/user/usage.md)
</cite>

## 目录
1. [简介](#简介)
2. [命令行选项](#命令行选项)
3. [使用示例](#使用示例)
4. [输出文件结构](#输出文件结构)
5. [错误处理与退出码](#错误处理与退出码)
6. [性能建议](#性能建议)

## 简介
`stat` 子命令是 FastQTools 工具集中的核心功能之一，专门用于对 FASTQ 格式的测序数据文件进行深度统计分析。该命令能够高效地解析 FASTQ 文件，计算并输出一系列关键的质控指标，包括读数数量、长度分布、碱基质量（Q20/Q30）、GC 含量等，为生物信息学分析提供重要的数据支持。

该命令的设计遵循高性能和模块化原则，利用 Intel TBB (Threading Building Blocks) 构建并行处理流水线，能够充分利用多核 CPU 资源，显著提升对大型 FASTQ 文件（尤其是压缩的 `.fastq.gz` 文件）的处理速度。其核心逻辑位于 `src/cli/commands/stat_command.cpp` 中，通过解析命令行参数，配置 `StatisticOptions` 结构体，并调用 `createStatisticCalculator` 工厂函数来创建具体的统计计算器实例。

**Section sources**
- [stat_command.cpp](file://src/cli/commands/stat_command.cpp#L11-L67)
- [stat_command.h](file://src/cli/commands/stat_command.h#L18-L41)
- [README.md](file://README.md#L23-L24)

## 命令行选项
`stat` 命令提供了丰富的命令行选项，允许用户精确控制分析过程。所有选项均通过 `cxxopts` 库进行解析，确保了良好的用户体验和健壮的参数验证。

| 选项 | 全称 | 数据类型 | 默认值 | 描述 |
| :--- | :--- | :--- | :--- | :--- |
| `-i` | `--input` | `string` | 必需 | 指定输入的 FASTQ 文件路径。支持未压缩的 `.fastq` 和 GZIP 压缩的 `.fastq.gz` 文件。 |
| `-o` | `--output` | `string` | 必需 | 指定输出统计结果文件的路径。结果将以制表符分隔的文本格式写入。 |
| `-t` | `--threads` | `size_t` | `1` | 设置用于并行处理的工作线程数量。增加线程数可以显著提升大文件的处理速度。 |
| | `--batch-size` | `size_t` | `50000` | 设置每个数据批次中包含的读段（read）数量。较大的批次可以提高内存利用率，但会增加单次处理的内存开销。 |
| | `--read-chunk-bytes` | `size_t` | `1048576` (1MB) | 设置从文件中读取数据的块大小（以字节为单位）。影响 I/O 效率。 |
| | `--batch-capacity-bytes` | `size_t` | `4194304` (4MB) | 设置单个批次缓冲区的容量上限（以字节为单位）。用于内存管理。 |
| | `--zlib-buffer-bytes` | `size_t` | `131072` (128KB) | 设置 zlib 解压缩库内部使用的缓冲区大小（以字节为单位）。 |
| | `--in-flight` | `size_t` | `0` (自动) | 设置流水线中最大“飞行中”（in-flight）的批次数量。`0` 表示由系统自动管理。 |
| | `--memory-limit-gb` | `size_t` | `10` | 设置用于“飞行中”批次的内存上限（以 GB 为单位）。`0` 表示无限制。该选项会与 `--in-flight` 选项共同作用，以防止内存溢出。 |
| `-h` | `--help` | `flag` | N/A | 打印该命令的详细帮助信息并退出。 |

这些选项的定义和默认值在 `stat_command.cpp` 的 `execute` 方法中通过 `cxxopts::Options` 对象进行配置，并最终映射到 `StatisticOptions` 结构体中，该结构体定义在 `include/fqtools/statistics/statistic_calculator_interface.h` 中。

**Section sources**
- [stat_command.cpp](file://src/cli/commands/stat_command.cpp#L13-L33)
- [statistic_calculator_interface.h](file://include/fqtools/statistics/statistic_calculator_interface.h#L29-L40)

## 使用示例
以下是一些 `stat` 命令的典型使用场景。

### 单文件分析
对单个 FASTQ 文件进行基本的统计分析。
```bash
FastQTools stat -i sample.fastq.gz -o sample_stats.txt
```

### 多线程处理
利用 8 个线程对大型 FASTQ 文件进行快速分析。
```bash
FastQTools stat -i large_dataset.fastq.gz -o analysis.txt -t 8
```

### 自定义批处理大小
调整批处理大小以优化特定硬件环境下的性能。
```bash
FastQTools stat -i data.fq -o result.stat.txt --batch-size 100000 --threads 4
```

### 结合环境变量的完整脚本
项目提供的 `examples/basic_usage/basic_stats.sh` 脚本展示了如何在生产环境中使用 `stat` 命令。该脚本不仅执行分析，还包含了文件检查、性能计时、结果摘要和 HTML 报告生成等完整的流程。

```bash
# 设置环境变量
export FASTQTOOLS_THREADS=8
export FASTQTOOLS_BATCH_SIZE=100000

# 运行示例脚本
./examples/basic_usage/basic_stats.sh input.fastq.gz my_output_prefix
```

**Section sources**
- [basic_stats.sh](file://examples/basic_usage/basic_stats.sh#L104-L108)
- [usage.md](file://docs/user/usage.md#L22-L25)

## 输出文件结构
`stat` 命令生成的输出文件是一个制表符分隔的文本文件，包含多个部分，以 `#` 开头的行作为元数据和列标题。

### 文件头部
包含输入文件的基本信息和全局统计。
```
#Name    sample.fastq.gz
#PhredQual    33
#ReadNum    1000000
#MaxReadLength    150
#BaseCount    150000000
```

### 核心质量与碱基统计
提供全局的 Q20、Q30、碱基组成和 GC 含量信息。
```
#Q20(>=20)    148500000    99.00%
#Q30(>=30)    145500000    97.00%
#A    37500000    25.00%
#C    37500000    25.00%
#G    37500000    25.00%
#T    37500000    25.00%
#N    0    0.00%
#GC    75000000    50.00%
```

### 位置特异性统计
从 `#Pos` 行开始，提供每个读段位置（1-based）的详细统计，包括各碱基的数量、平均质量分数和错误率。
```
#Pos    A    C    G    T    N    AvgQual    ErrRate
1    1000000    0    0    0    0    38.5    0.00014
2    999000    1000    0    0    0    38.2    0.00015
...
150    0    0    0    1000000    0    30.1    0.00098
```

此输出结构的生成逻辑在 `fq_statistic.cpp` 的 `writeResult` 方法中实现，该方法遍历聚合后的 `FqStatisticResult` 对象，计算并格式化所有统计数据。

**Section sources**
- [fq_statistic.cpp](file://src/statistics/fq_statistic.cpp#L160-L256)

## 错误处理与退出码
`stat` 命令具备完善的错误处理机制，能够优雅地处理各种异常情况。

*   **文件不可读**: 如果指定的输入文件不存在或无法打开，程序会抛出 `std::runtime_error` 异常，并在日志中记录错误信息，然后以非零退出码终止。
*   **输出路径无效**: 如果指定的输出目录不存在或没有写入权限，同样会抛出 `std::runtime_error` 异常。
*   **参数缺失**: 如果未提供必需的 `-i` 或 `-o` 参数，`cxxopts` 库会自动捕获并提示错误，程序将返回非零退出码。
*   **内存不足**: 当处理超大文件时，如果内存使用超过 `--memory-limit-gb` 的限制，系统会通过限制“飞行中”批次的数量来尝试避免崩溃。如果仍无法处理，可能会因内存分配失败而终止。

**退出码**:
*   **0**: 命令执行成功，所有分析完成。
*   **非零值**: 命令执行失败，原因可能是参数错误、文件 I/O 错误或运行时异常。

错误处理逻辑主要体现在 `fq_statistic.cpp` 的 `run` 方法中，例如在创建 `FastqReader` 后会立即检查 `isOpen()` 状态，若失败则抛出异常。

**Section sources**
- [fq_statistic.cpp](file://src/statistics/fq_statistic.cpp#L117-L119)
- [fq_statistic.cpp](file://src/statistics/fq_statistic.cpp#L162-L164)

## 性能建议
为了获得最佳的分析性能，请遵循以下建议：

1.  **使用多线程**: 对于大型文件，务必使用 `-t` 或 `--threads` 选项指定一个与 CPU 核心数相匹配的线程数。这是提升性能最有效的方法。
2.  **调整批处理大小**: `--batch-size` 和 `--batch-capacity-bytes` 是影响内存和 CPU 缓存效率的关键参数。对于内存充足的系统，适当增加这些值可以减少线程间的同步开销。
3.  **监控内存使用**: 使用 `--memory-limit-gb` 选项可以防止程序因占用过多内存而被操作系统终止（OOM killed），尤其是在共享计算环境中。
4.  **选择合适的 I/O 块大小**: `--read-chunk-bytes` 的设置应与存储设备的 I/O 特性相匹配。对于 SSD，较大的块大小通常更高效。

通过合理配置这些参数，`stat` 命令能够高效地处理从几 MB 到数百 GB 的 FASTQ 数据。

**Section sources**
- [fq_statistic.cpp](file://src/statistics/fq_statistic.cpp#L93-L108)
- [README.md](file://README.md#L28-L29)