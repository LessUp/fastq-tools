# FastQTools Optimization Design

本文档描述了 FastQTools 各项优化的具体技术设计。

## 1. IO 系统设计

### 1.1 无拷贝 Remainder 处理
- **方案**: 引入 "Sliding Buffer" 机制。
- **细节**: 在 `FastqReader` 内部维护一个较大的缓冲区。当上次读取剩余半条记录时，不再将其拷贝到专门的 `remainder` 容器，而是将剩余部分通过 `std::memmove` 移至缓冲区首部（或者由于缓冲区足够大，直接在剩余部分后面继续 `gzread`）。
- **优化点**: 减少一次 `std::string::assign` 的物理拷贝，降低 CPU 和内存总线压力。

### 1.2 libdeflate 集成架构
- **方案**: 封装 `CompressionStream` 抽象接口。
- **细节**:
  - 创建 `GzipDecompressor` 和 `GzipCompressor` 接口。
  - 提供 `ZlibImpl` 和 `LibdeflateImpl` 两种实现。
  - 在编译时通过 CMake 检测 `libdeflate`，优先链接。
- **优势**: `libdeflate` 的解压速度通常比 `zlib` 快 2-3 倍，能显著提升解压瓶颈。

### 1.3 Huge Pages 内存管理
- **方案**: 自定义 `HugePageAllocator`。
- **细节**: 使用 `mmap(..., MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0)` 分配 IO 缓冲区。
- **回退机制**: 若系统未开启 Huge Pages，则回退到 `posix_memalign` 或标准 `malloc`。

## 2. 计算加速设计

### 2.1 SIMD 向量化方案
- **技术栈**: 使用封装库（如 `xsimd`）或直接使用原生 Intrinsics。
- **应用场景**:
  - **Quality Checking**: 使用 `_mm_cmpgt_epi8` 等指令一次性判断 16/32 个碱基的质量。
  - **Base Counting**: 使用映射表加速碱基统计。
  - **Mismatch Counting**: 使用 `_mm_xor_si128` 和 `_mm_test_all_zeros` 快速比较序列差异。

### 2.2 多线程并行解析 (Parallel Parsing)
- **架构**: 生产者-消费者模型。
- **流程**:
  1. Serial Step: 从文件读取原始块到内存。
  2. Parallel Step: 将大块切分为多个子块，多个 Worker 同时查找子块内的 FASTQ 记录边界并解析。
  3. Serial Step: 按顺序合并记录，推送到处理管道。

## 3. 插件系统设计

### 3.1 插件接口定义
- **方案**: 定义 C ABI 兼容的入口函数。
- **细节**:
```cpp
extern "C" {
    ReadMutatorInterface* create_mutator();
    const char* get_plugin_version();
}
```
- **加载器**: `PluginLoader` 类负责 `dlopen`、获取符号并管理插件生命周期。

## 4. 日志系统优化
- **方案**: 切换到 `spdlog::async_logger`。
- **配置**: 创建一个全局线程池（Thread Pool），所有的 Log Sinks 挂载在异步 Logger 下，避免 IO 阻塞主逻辑。
