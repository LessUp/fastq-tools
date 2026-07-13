# Callgrind 热点分析快照

| 字段 | 值 |
|------|----|
| 日期 | 2026-07-13 |
| 工具 | Valgrind 3.22.0 / Callgrind |
| 构建类型 | RelWithDebInfo（-O2 + -g） |
| 数据集 | sample_1k（1K reads × 100 bp） |
| 基线 | [environment.md](../../environment.md) |

## 调用方式

```bash
valgrind --tool=callgrind --collect-jumps=yes \
  --callgrind-out-file=stat-callgrind.out \
  build/clang-relwithdebinfo/FastQTools stat -i /tmp/sample_1k.fastq -o /dev/null

callgrind_annotate --auto=yes --inclusive=yes stat-callgrind.out > stat-callgrind-annotate.txt
```

## stat 命令热点（inclusive，按 Ir 排序）

总指令数：**13,564,161**

| 排名 | 函数 | Ir | 占比 |
|------|------|----|------|
| 1 | `StatCommand::execute` | 10,973,908 | 80.90% |
| 2 | `FastqStatisticCalculator::run` | 10,305,159 | 75.97% |
| 3 | `ExecutionRuntime::executeErased` | 8,618,618 | 63.54% |
| 4 | `SequentialExecutionBackend::execute` | 8,612,356 | 63.49% |
| 5 | `FqStatisticWorker::calculateStats` | 6,092,789 | 44.92% |
| 6 | `FastqReader::nextBatch` | 2,444,828 | 18.02% |
| 7 | `__memset_avx2_unaligned_erms` | 2,176,027 | 16.04% |
| 8 | `FastqReader::nextBatch`（construct_at 内联） | 2,110,198 | 15.56% |
| 9 | `_dl_start`（动态链接器，启动开销） | 1,891,470 | 13.94% |
| 10 | `FqStatisticWorker::calculateStats`（char_traits 内联） | 1,729,915 | 12.75% |

### 关键发现

- **统计计算占主导**：`FqStatisticWorker::calculateStats` 占 44.92%，是 stat 命令的核心热点。
- **IO 读取次之**：`FastqReader::nextBatch` 占 18.02%，其中 `__memset_avx2`（16.04%）是 batch 初始化清零开销。
- **动态链接器开销**：`_dl_start` 等占 ~14%，这是启动一次性开销，与数据处理无关，大数据集下占比会下降。
- **memset 占比高**：16.04% 的指令花在 memset，来自 batch 容器初始化。可考虑对象池复用减少清零。

## filter 命令热点（inclusive，按 Ir 排序）

总指令数：**6,501,671**

| 排名 | 函数 | Ir | 占比 |
|------|------|----|------|
| 1 | `FilterCommand::execute` | 3,909,968 | 60.14% |
| 2 | `ProcessingPipeline::run` | 2,901,954 | 44.63% |
| 3 | `ExecutionRuntime::executeErased` | 2,901,212 | 44.62% |
| 4 | `SequentialExecutionBackend::execute` | 2,893,688 | 44.51% |
| 5 | `FastqReader::nextBatch` | 2,444,828 | 37.60% |
| 6 | `__memset_avx2_unaligned_erms` | 2,200,728 | 33.85% |
| 7 | `FastqReader::nextBatch`（construct_at 内联） | 2,110,198 | 32.46% |
| 8 | `_dl_start`（动态链接器） | 1,891,470 | 29.09% |
| 9 | `do_lookup_x`（动态符号查找） | 896,205 | 13.78% |
| 10 | `cxxopts::OptionAdder`（CLI 解析） | 757,382 | 11.65% |

### 关键发现

- **filter 比 stat 轻量**：filter 总指令数 6.5M vs stat 13.6M。filter 不做统计计算，只做谓词判断 + 写出。
- **IO 读取是 filter 主热点**：`FastqReader::nextBatch` 占 37.60%，filter 的计算开销小，IO 占比相对放大。
- **memset 占比更高**：33.85%，filter 路径里 batch 初始化清零是最大单项开销。
- **CLI 解析开销显著**：`cxxopts` 占 11.65%，这是启动一次性开销。
- **动态链接器开销占比大**：~29%，因为 filter 总工作量小，启动开销被放大。大数据集下会摊薄。

## 跨命令对比

| 指标 | stat | filter |
|------|------|--------|
| 总指令数 | 13,564,161 | 6,501,671 |
| stat 计算 | 44.92% | — |
| IO 读取 | 18.02% | 37.60% |
| memset（batch 初始化） | 16.04% | 33.85% |
| 动态链接器 | 13.94% | 29.09% |
| CLI 解析 | <1% | 11.65% |

## 优化方向

1. **memset/batch 初始化**：stat 和 filter 都有 16-34% 指令花在 memset。对象池复用 batch 容器可减少清零开销。
2. **统计计算路径**：stat 的 44.92% 在 `calculateStats`，是算法级优化的目标（如 SIMD 向量化质量统计）。
3. **启动开销**：动态链接器 + CLI 解析在小数据集下占比高，大数据集下摊薄，非优化重点。

## 原始数据

- [stat-callgrind.out](./stat-callgrind.out.gz) — Callgrind 原始数据
- [stat-callgrind-annotate.txt](./stat-callgrind-annotate.txt.gz) — 完整 annotate 报告
- [filter-callgrind.out](./filter-callgrind.out.gz) — Callgrind 原始数据
- [filter-callgrind-annotate.txt](./filter-callgrind-annotate.txt.gz) — 完整 annotate 报告
