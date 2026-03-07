# Design Document: Memory Pool for FastqBatch

## Overview

本设计文档描述了 FastQTools 项目中 `ObjectPool<T>` 模板类的技术设计，用于高效复用 `FastqBatch` 对象，减少并行处理流水线中的内存分配开销。

## Architecture

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

## Components and Interfaces

### ObjectPool<T> 模板类

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

private:
    std::vector<std::unique_ptr<T>> pool_;
    mutable std::mutex mutex_;
    ResetFunc resetFunc_;
    size_t maxSize_;
    std::atomic<size_t> activeCount_{0};
    std::atomic<size_t> totalCreated_{0};
};

} // namespace fq::memory
```

### FastqBatchPool

```cpp
namespace fq::io {

using FastqBatchPool = fq::memory::ObjectPool<FastqBatch>;

inline void resetFastqBatch(FastqBatch& batch) {
    batch.clear();
}

inline auto createFastqBatchPool(size_t initialSize, size_t maxSize)
    -> std::shared_ptr<FastqBatchPool>;

} // namespace fq::io
```

## Implementation Details

### 自动归还机制

使用 shared_ptr 自定义删除器实现自动归还：

```cpp
auto weakThis = this->shared_from_this();
return std::shared_ptr<T>(obj.release(), [weakThis](T* ptr) {
    if (auto pool = weakThis.lock()) {
        pool->releaseImpl(std::unique_ptr<T>(ptr));
    } else {
        delete ptr;  // 池已销毁，直接删除
    }
});
```

### Pipeline 集成

```cpp
auto batchPool = fq::io::createFastqBatchPool(maxTokens, maxTokens * 2);

tbb::make_filter<void, std::shared_ptr<fq::io::FastqBatch>>(
    tbb::filter_mode::serial_in_order,
    [reader, batchPool, this](tbb::flow_control& fc) {
        auto batch = batchPool->acquire();
        if (reader->nextBatch(*batch, config_.batchSize)) {
            return batch;
        }
        fc.stop();
        return nullptr;
    })
```

## Error Handling

| 场景 | 处理方式 |
|------|----------|
| 内存分配失败 | 抛出 std::bad_alloc |
| 池已销毁时释放 | 安全删除对象 |

## Testing Strategy

### 单元测试

- 基础功能: acquire, release, reserve, shrink
- 监控方法正确性
- 对象重置功能
- 容量限制功能
- 并发获取/释放
