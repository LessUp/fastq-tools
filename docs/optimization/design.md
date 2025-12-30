# FastQTools 优化设计（精简版）

本文档描述已实现优化的技术设计。

## 1. 内存池设计

### 1.1 设计目标

减少 TBB parallel_pipeline 中 FastqBatch 对象的频繁创建和销毁开销。

### 1.2 架构

```
┌─────────────────────────────────────────────────────────────┐
│                    Processing Pipeline                       │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐     │
│  │   Source    │───▶│  Processing │───▶│    Sink     │     │
│  │   Stage     │    │   Stage     │    │   Stage     │     │
│  └──────┬──────┘    └─────────────┘    └──────┬──────┘     │
│         │                                      │            │
│         │ acquire()              auto release  │            │
│         ▼                                      ▼            │
│  ┌─────────────────────────────────────────────────────────┐│
│  │                 ObjectPool<FastqBatch>                  ││
│  └─────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────┘
```

### 1.3 核心接口

```cpp
namespace fq::memory {

template <typename T>
class ObjectPool : public std::enable_shared_from_this<ObjectPool<T>> {
public:
    using ResetFunc = std::function<void(T&)>;
    
    explicit ObjectPool(size_t initialSize = 0, 
                        size_t maxSize = 0,
                        ResetFunc resetFunc = nullptr);
    
    [[nodiscard]] auto acquire() -> std::shared_ptr<T>;
    void reserve(size_t count);
    void shrink() noexcept;
    
    [[nodiscard]] auto poolSize() const -> size_t;
    [[nodiscard]] auto activeCount() const -> size_t;
    [[nodiscard]] auto totalCreated() const -> size_t;
};

} // namespace fq::memory
```

### 1.4 关键实现细节

1. **自动归还**: 使用 shared_ptr 自定义删除器，对象析构时自动归还池中
2. **线程安全**: 使用 mutex 保护池操作
3. **容量限制**: 支持 maxSize 限制，超出时丢弃对象
4. **对象重置**: 获取时调用 resetFunc 确保对象干净

### 1.5 Pipeline 集成

```cpp
// processWithTBB() 中
auto batchPool = fq::io::createFastqBatchPool(maxTokens, maxTokens * 2);

tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
    tbb::filter_mode::serial_in_order,
    [reader, batchPool, this](tbb::flow_control& fc) {
        auto batch = batchPool->acquire();  // 从池获取
        if (reader->nextBatch(*batch, config_.batchSize)) {
            return batch;
        }
        fc.stop();
        return nullptr;
    })
// shared_ptr 析构时自动归还，无需显式 release
```

## 2. libdeflate 集成

### 2.1 设计目标

利用 libdeflate 更高效的解压算法提升 gzip 文件处理性能。

### 2.2 集成方式

- 通过 Conan 包管理器引入 libdeflate 依赖
- 在 FastqReader 中使用 libdeflate 替代 zlib 进行解压
- 保持 API 兼容，对上层透明

### 2.3 性能收益

- 解压速度提升 2-3 倍
- 内存使用基本持平
- 无需修改上层代码

## 3. 设计决策记录

### 3.1 为什么不实现 SIMD 优化

- 需要大量平台特定代码 (SSE4.2, AVX2, NEON)
- 维护成本高，需要多套实现
- 当前瓶颈在 IO 和解压，不在计算
- 可在 profiling 证明必要后再考虑

### 3.2 为什么不实现动态插件系统

- dlopen 机制增加 ABI 兼容性负担
- 需要维护稳定的 C ABI 接口
- 当前用户场景不需要运行时扩展
- 静态编译更简单可靠

### 3.3 为什么不实现并行解析

- TBB parallel_pipeline 已提供足够的并行度
- 并行解析需要复杂的记录边界检测
- 增加代码复杂度但收益有限
- 当前瓶颈在解压而非解析

## 4. 性能验证

建议的 benchmark 方法：

```bash
# 对比有/无内存池的性能
time fqtools filter -i large.fq.gz -o out.fq.gz -t 8

# 使用 perf 分析热点
perf record -g fqtools filter -i large.fq.gz -o out.fq.gz -t 8
perf report
```

关注指标：
- 吞吐量 (MB/s)
- 内存分配次数
- CPU 利用率
