# Google Benchmark FASTQ IO 吞吐快照

| 字段 | 值 |
|------|----|
| 日期 | 2026-07-13 |
| 工具 | Google Benchmark 1.9.5 |
| 构建类型 | Release（-O3，LTO） |
| 重复次数 | 3（报告 mean / median / stddev / cv） |
| 基线 | [environment.md](../../../environment.md) |

## 调用方式

```bash
cmake -S . -B build/clang-release -DBUILD_BENCHMARKS=ON
cmake --build build/clang-release --target benchmark_fastq_io
./build/clang-release/tools/benchmark/benchmark_fastq_io \
  --benchmark_format=json \
  --benchmark_out=fastq_io_results.json \
  --benchmark_repetitions=3 \
  --benchmark_report_aggregates_only=true
```

## Reader 吞吐

| Benchmark | 数据集 | mean 时间 | 吞吐 | 读取速率 |
|-----------|--------|-----------|------|----------|
| BM_FastQReader_Small | 10K reads | 0.973 ms | 3.48 GB/s | 10.84 M reads/s |
| BM_FastQReader_Medium | 100K reads | 10.356 ms | 3.10 GB/s | 9.67 M reads/s |
| BM_FastQReader_Parameterized/10000 | 10K reads | 0.880 ms | 3.67 GB/s | 11.45 M reads/s |
| BM_FastQReader_Parameterized/50000 | 50K reads | 5.334 ms | 3.02 GB/s | 9.40 M reads/s |
| BM_FastQReader_Parameterized/100000 | 100K reads | 10.854 ms | 2.97 GB/s | 9.27 M reads/s |

### Reader 分析

- **吞吐 3.0-3.7 GB/s**：纯读取（无处理），接近内存带宽上限。
- **小数据集吞吐更高**：10K reads 3.67 GB/s vs 100K reads 2.97 GB/s。小数据集 L1/L2 命中率高，大数据集 cache miss 增加导致吞吐略降。
- **读取速率 9.3-11.5 M reads/s**：每秒近千万条 reads 解析，满足高通量测序数据处理需求。
- **线性扩展**：10K→100K reads，时间从 0.88ms→10.85ms，接近线性（12.3x for 10x data），开销主要在数据本身。

## Writer 吞吐

| Benchmark | 数据集 | mean 时间 | 写入速率 |
|-----------|--------|-----------|----------|
| BM_FastQWriter_Small | 10K reads | 25.413 ms | 398.50 K reads/s |
| BM_FastQWriter_Medium | 100K reads | 232.185 ms | 432.51 K reads/s |
| BM_FastQWriter_Parameterized/10000 | 10K reads | 24.188 ms | 421.04 K reads/s |
| BM_FastQWriter_Parameterized/50000 | 50K reads | 119.293 ms | 423.57 K reads/s |
| BM_FastQWriter_Parameterized/100000 | 100K reads | 245.685 ms | 406.89 K reads/s |

### Writer 分析

- **写入速率 ~420 K reads/s**：远低于读取速率（10M reads/s），写是性能瓶颈。
- **Writer 比 Reader 慢 ~25x**：Reader 10.84M reads/s vs Writer 398K reads/s。Writer 涉及格式化输出 + I/O 系统调用，开销大。
- **线性扩展**：10K→100K reads，时间从 24ms→246ms，线性扩展，无异常。

## 稳定性

| Benchmark | cv（变异系数） |
|-----------|----------------|
| BM_FastQReader_Small | 24.7% |
| BM_FastQReader_Medium | 7.4% |
| BM_FastQReader_Parameterized/10000 | 9.5% |
| BM_FastQReader_Parameterized/100000 | — |
| BM_FastQWriter_Small | 1.6% |
| BM_FastQWriter_Medium | 0.8% |

- **Reader 小数据集 cv 高（24.7%）**：10K reads 运行时间 <1ms，测量噪声占比大。100K reads cv 降到 7.4%，更稳定。
- **Writer cv 极低（<2%）**：Writer 运行时间长（25-250ms），测量稳定。

## 关键发现

1. **Reader 性能优秀**：3.0-3.7 GB/s 吞吐，接近内存带宽上限，零拷贝 `string_view` 设计有效。
2. **Writer 是瓶颈**：420K reads/s vs Reader 10M reads/s，慢 25x。Writer 路径是优化重点。
3. **小数据集测量噪声大**：Reader 小数据集 cv 24.7%，建议用 100K+ reads 做基准。
4. **线性扩展**：Reader 和 Writer 都线性扩展，无 O(n²) 退化。

## 优化方向

1. **Writer 路径**：当前 420K reads/s，可考虑：
   - 批量 write 系统调用（减少 syscall 次数）
   - 格式化缓冲区复用
   - 可选 gzip 压缩流式写入
2. **Reader 大数据集 cache 优化**：100K reads 吞吐比 10K 低 20%，batch 大小调优可能改善。

## 原始数据

- [fastq_io_results.json](./fastq_io_results.json) — Google Benchmark JSON 输出
- [fastq_io_console.txt](./fastq_io_console.txt) — 控制台完整输出
