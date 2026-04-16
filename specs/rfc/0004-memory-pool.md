# RFC-0004: Memory Pool Optimization

> **Status**: Active  
> **Created**: 2026-04-17  
> **Last Updated**: 2026-04-17  
> **Authors**: FastQTools Core Team  
> **Supersedes**: `docs/specs/memory-pool/design.md`

## Context

FASTQ processing involves frequent allocation and deallocation of `FastqBatch` objects during pipeline execution. A memory pool can reduce allocation overhead and improve performance by reusing batch objects.

## Decision

### Architecture

Implement `ObjectPool<T>` template for `FastqBatch` reuse with automatic return mechanism via `shared_ptr` custom deleter:

```
┌─────────────────────────────────────────────────────────┐
│                   ObjectPool<T>                          │
├─────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────┐   │
│  │              Pool Storage                         │   │
│  │  • Free list (stack-based)                       │   │
│  │  • Pre-allocated blocks                          │   │
│  │  • Thread-safe access (TBB concurrent_queue)    │   │
│  └──────────────────────────────────────────────────┘   │
│                          │                               │
│                          ▼                               │
│  ┌──────────────────────────────────────────────────┐   │
│  │          Acquisition Interface                    │   │
│  │  • acquire() → shared_ptr<T> with custom deleter │   │
│  │  • Automatic return to pool on refcount=0        │   │
│  └──────────────────────────────────────────────────┘   │
│                          │                               │
│                          ▼                               │
│  ┌──────────────────────────────────────────────────┐   │
│  │        FastqBatchPool Specialization              │   │
│  │  • Optimized for FastqBatch size                  │   │
│  │  • Pipeline integration helpers                   │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### Interface Design

```cpp
namespace fq::memory {

template<typename T>
class ObjectPool {
public:
    explicit ObjectPool(size_t initialCapacity = 64);
    ~ObjectPool();

    // Acquire object from pool, auto-returns on destruction
    auto acquire() -> std::shared_ptr<T>;

    // Pool statistics
    auto getActiveCount() const -> size_t;
    auto getFreeCount() const -> size_t;
    auto getTotalAllocated() const -> size_t;

    // Disable copy/move
    ObjectPool(const ObjectPool&) = delete;
    auto operator=(const ObjectPool&) = delete;
};

// FastqBatch specialization
class FastqBatchPool {
public:
    static auto getInstance() -> FastqBatchPool&;

    auto acquireBatch() -> std::shared_ptr<FastqBatch>;

    // Configuration
    void setPoolSize(size_t size);
    void preallocate(size_t count);
};

}  // namespace fq::memory
```

### Auto-Return Mechanism

```cpp
auto ObjectPool<T>::acquire() -> std::shared_ptr<T> {
    T* obj = /* get from free list or allocate */;
    return std::shared_ptr<T>(obj, [this](T* ptr) {
        // Custom deleter returns to pool instead of deleting
        returnToPool(ptr);
    });
}
```

### Pipeline Integration

```cpp
// In processing pipeline
auto pool = memory::FastqBatchPool::getInstance();

auto processStage = [pool](auto batch) {
    auto newBatch = pool->acquireBatch();
    // Process data...
    return newBatch;  // Old batch auto-returned to pool
};
```

### Correctness Properties

#### Property 1: Object Reuse

*For any* object acquired from the pool and released (refcount reaches 0), the object SHALL be returned to the pool's free list and made available for subsequent `acquire()` calls.

#### Property 2: Thread Safety

*For any* concurrent `acquire()` calls from multiple threads, the pool SHALL NOT have data races and each call SHALL return a distinct object.

#### Property 3: Memory Bounding

*For any* pool configuration with maximum capacity N, total allocated memory SHALL NOT exceed `N * sizeof(T)`.

#### Property 4: Pre-allocation Correctness

*For any* pre-allocation request of size N, the pool SHALL allocate N objects upfront and subsequent `acquire()` calls SHALL use these pre-allocated objects until exhausted.

### Error Handling

| Error Type | Handling |
|-----------|----------|
| Pool exhausted | Allocate new object (pool grows beyond initial size) |
| Thread contention | Use lock-free queue or exponential backoff |
| Invalid pool size | Throw `ConfigurationError` with valid range |

### Testing Strategy

1. **Unit Tests**: Pool allocation/deallocation, statistics accuracy, thread safety
2. **Property-Based Tests**: Object reuse invariant, memory bounding, pre-allocation correctness
3. **Integration Tests**: Pipeline integration, performance comparison with/without pool
4. **Benchmark Tests**: Allocation throughput comparison, memory usage patterns

## Consequences

### Positive
- Reduced allocation overhead for batch processing
- Improved cache locality with pre-allocated blocks
- Automatic lifetime management via smart pointers
- Transparent integration with existing pipeline

### Negative
- Added complexity to memory management
- Pool memory not released until pool destruction
- Thread contention under high parallelism

## Related Documents

- [RFC-0001: Core Architecture](0001-core-architecture.md)
- [Product Specification](../product/fastq-processing.md)
