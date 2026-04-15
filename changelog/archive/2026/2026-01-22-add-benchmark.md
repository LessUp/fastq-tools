# 2026-01-22: 添加 Pipeline 和 ObjectPool Benchmark

## 新增功能

### 新增 Benchmark 测试

- 添加了 `pipeline_benchmark.cpp`：测试处理流水线端到端性能
  - 串行处理 benchmark
  - 并行处理 benchmark（2/4/8 线程）
  - 覆盖 10K/50K/100K reads 场景

- 添加了 `object_pool_benchmark.cpp`：测试对象池性能
  - 有池化 vs 无池化对比
  - FastqBatch 池化测试
  - 多线程并发测试

### 更新 CMakeLists.txt

- 在 `tools/benchmark/CMakeLists.txt` 中注册新 benchmark
- 更新 `ALL_BENCHMARK_TARGETS` 列表

## 性能结果

### Pipeline Benchmark (100K reads)

| 模式 | 吞吐量 | 处理时间 |
|------|--------|----------|
| 串行 | 475 MB/s | 63.4 ms |
| 2 线程 | 668 MB/s | 45.1 ms |
| 4 线程 | 706 MB/s | 42.7 ms |
| 8 线程 | 591 MB/s | 50.9 ms |

**结论**：4线程并行比串行提升 48%，8线程因 IO 瓶颈略有下降。

### ObjectPool Benchmark

| 场景 | 性能 |
|------|------|
| 单线程借出/归还 | 6.9M ops/s |
| 2 线程并发 | 1.46M ops/s |
| 4 线程并发 | 1.64M ops/s |

**结论**：对象池在高频场景下表现稳定，mutex 开销可接受。

## 影响的文件

- `tools/benchmark/pipeline_benchmark.cpp` (新增)
- `tools/benchmark/object_pool_benchmark.cpp` (新增)
- `tools/benchmark/CMakeLists.txt` (修改)
