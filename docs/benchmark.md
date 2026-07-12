# 性能基准

## 测试环境

- CPU：AMD Ryzen 9 5900X
- 数据：100K reads，150 bp
- 构建：Clang Release

## 代表性结果

| 工作负载 | 结果 |
|----------|------|
| FASTQ 读取路径 | 1696 MB/s |
| FASTQ 写出路径 | 1.76M reads/s |
| 组合过滤处理 | 1.67M reads/s |
| 完整统计分析 | 302 MB/s |

## 解读

- **读取路径 1696 MB/s**：接近 gzip 解压 + 磁盘 I/O 瓶颈。零拷贝 `string_view` 视图让解析退化为指针算术，不构成瓶颈。
- **写出路径 1.76M reads/s**：gzip 压缩是主要成本。批量写入分摊系统调用开销。
- **组合过滤 1.67M reads/s**：过滤 + 修剪单遍扫描，额外 CPU 开销小，吞吐接近纯写出。
- **统计 302 MB/s**：低于其他路径，因为统计是 CPU 密集（逐碱基质量统计、GC 滑窗、长度直方图），且无写出路径分摊。

## 基准测试工具

```bash
# 构建基准
cmake --build build --target benchmarks

# 运行单个
./build/tools/benchmark/benchmark_fastq_io --benchmark_format=json

# 运行全部
cmake --build build --target run_benchmarks
```

基准套件在 `tools/benchmark/`，基于 Google Benchmark：

| 文件 | 测试内容 |
|------|----------|
| `fastq_io_benchmark.cpp` | 读写吞吐量 |
| `filter_benchmark.cpp` | filter 端到端 |
| `stat_benchmark.cpp` | stat 计算性能 |
| `pipeline_benchmark.cpp` | TBB 流水线吞吐 |
| `object_pool_benchmark.cpp` | 对象池分配性能 |

## 注意

这些数字是维护中的点时快照，适合判断量级，不是所有数据集、压缩级别或存储环境下的绝对承诺。
