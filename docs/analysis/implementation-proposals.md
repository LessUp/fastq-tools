# 优化实现方案（精简版）

> 本文档描述经过辩证分析后保留的实际可行优化方案。

---

## 1. 内存池实现方案 ✅ 已完成

### 1.1 设计目标

- 减少 `FastqBatch` 对象的频繁分配
- 支持多线程安全访问
- 通过 shared_ptr 自定义删除器实现自动归还

### 1.2 接口设计

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

### 1.3 Pipeline 集成

```cpp
// 创建对象池
auto batchPool = fq::io::createFastqBatchPool(maxTokens, maxTokens * 2);

// Source stage 使用对象池
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
// Sink stage 无需显式释放，shared_ptr 析构时自动归还
```

---

## 2. libdeflate 集成 ✅ 已完成

项目已通过 Conan 集成 libdeflate，用于替代 zlib 进行 gzip 解压，性能提升 2-3 倍。

---

## 3. 移除的过度设计

以下方案经评估后认为不适合当前项目：

| 方案 | 移除原因 |
|------|----------|
| SIMD 优化 | 平台特定代码多，维护成本高，当前瓶颈不在计算 |
| 并行解析 | TBB pipeline 已足够，增加复杂度收益有限 |
| 插件系统 | dlopen 增加 ABI 兼容负担，静态编译足够 |
| IO 抽象层 | 当前 FastqReader/Writer 已满足需求 |
| 属性测试 | 需要引入 RapidCheck 依赖，当前单元测试足够 |
| 模糊测试 | 需要额外工具链配置，可后续考虑 |
| Huge Pages | 需要系统配置，部署复杂 |
| NUMA 感知 | 过于复杂，大多数场景用不到 |

---

## 4. 实施状态

| 方案 | 状态 | 说明 |
|------|------|------|
| 内存池 | ✅ 完成 | 已集成到 Pipeline |
| libdeflate | ✅ 完成 | 已通过 Conan 集成 |

---

## 5. 后续优化建议

在 profiling 数据支持下，可考虑：

1. **SIMD 质量检查** - 如果质量过滤成为瓶颈
2. **io_uring** - 如果 IO 成为明确瓶颈且目标平台支持
3. **更细粒度的并行** - 如果 CPU 利用率不足
