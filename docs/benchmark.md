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

## 与同类工具对比

本表不内置 fastp/seqkit 的直接同环境数字，因为对比结果高度依赖 CPU、磁盘、压缩级别、参数与数据集，写死容易误导。下面给出**量级定位**与**可复现方法**，供你在自己的硬件上补一行。

| 工具 | 典型量级（公开基准） | 与 FastQTools 的定位差异 |
| --- | --- | --- |
| fastp | 多线程下数十 M reads/min（150bp PE） | 功能面远大于 FastQTools；FastQTools 聚焦 stat+filter 热点路径，内核更现代 |
| seqkit | Go 实现，单线程数十 M reads/min | 算子面巨大；FastQTools 原生 C++、二进制更小、可嵌入 |
| FastQTools | 读取 1696 MB/s，过滤 167 万 reads/s | 见上表 |

**如何在本仓库跑一次对比**：

```bash
# 1. 构建 FastQTools 基准
cmake --build build --target benchmarks

# 2. 跑 FastQTools 基准
./build/tools/benchmark/benchmark_fastq_io --benchmark_format=json

# 3. 在同一台机器、同一份数据上跑 fastp / seqkit
#    例如：fastp -i sample.fastq.gz -o /dev/null --thread 8
#         seqkit stats -Q sample.fastq.gz

# 4. 记录环境（CPU、磁盘、压缩级别、参数）后填入上表
```

公开基准来源（量级参考，非本仓库实测）：
- fastp：[OpenGene/fastp](https://github.com/OpenGene/fastp) README 与 issue 中的用户基准
- seqkit：[shenwei356/seqkit](https://github.com/shenwei356/seqkit) README 性能小节

## 注意

这些数字是维护中的点时快照，适合判断量级，不是所有数据集、压缩级别或存储环境下的绝对承诺。与同类工具的对比请在你自己的硬件上按上述方法复现，不要把本表数字当作跨工具的绝对结论。
