# FastQTools 优化决策记录

> 本文档整合了 FastQTools 项目的优化需求、辩证分析、技术设计和实施状态。
>
> 合并自：`optimization/requirements.md`、`optimization/design.md`、`optimization/implementation.md`、`optimization/critical-analysis.md`

---

## 设计原则

- **简单优先**: 避免过度设计，保持代码可维护性
- **收益明确**: 只实现有明确性能收益的优化
- **低侵入性**: 优化不应大幅改变现有架构
- **数据驱动**: 基于 profiling 数据决策

---

## 已实现的优化

### 1. 内存池 (Object Pool) ✅

**价值**: 减少 TBB pipeline 中 FastqBatch 对象的频繁分配/释放开销

**实现文件**:

| 文件 | 说明 |
|------|------|
| `include/fqtools/memory/object_pool.h` | 通用对象池模板类 |
| `include/fqtools/io/fastq_batch_pool.h` | FastqBatch 专用池 |
| `src/processing/processing_pipeline.cpp` | Pipeline 集成 |
| `tests/unit/memory/test_object_pool.cpp` | 单元测试 |

**架构设计**:

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

**核心接口**:

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

**关键实现细节**:

1. **自动归还**: 使用 shared_ptr 自定义删除器，对象析构时自动归还池中
2. **线程安全**: 使用 mutex 保护池操作
3. **容量限制**: 支持 maxSize 限制，超出时丢弃对象
4. **对象重置**: 获取时调用 resetFunc 确保对象干净

**Pipeline 集成**:

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

**测试覆盖**:

| 测试用例 | 状态 |
|----------|------|
| 空池创建新对象 | ✅ |
| 复用池中对象 | ✅ |
| 自动归还 | ✅ |
| 预分配 | ✅ |
| 监控方法 | ✅ |
| 对象重置 | ✅ |
| 容量限制 | ✅ |
| 并发获取/释放 | ✅ |
| 池销毁安全 | ✅ |

### 2. libdeflate 集成 ✅

**价值**: 解压速度比 zlib 快 2-3 倍，显著提升 gzip 文件处理性能

**实现文件**:

| 文件 | 说明 |
|------|------|
| `conanfile.py` | 依赖配置 |
| `src/io/fastq_reader.cpp` | 解压实现 |

**集成方式**:

- 通过 Conan 包管理器引入 libdeflate 依赖
- 在 FastqReader 中使用 libdeflate 替代 zlib 进行解压
- 保持 API 兼容，对上层透明

---

## 辩证分析与决策记录

### Remainder 处理优化 → 已使用 memmove 简化方案

- **正题**: 物理拷贝在高吞吐场景下累积效应不可忽视
- **反题**: Remainder 通常很小（几十到几百字节），相比 zlib 解压和磁盘 IO 开销微乎其微；实现无拷贝（Circular Buffer）会显著增加复杂度
- **合题**: 采用简单的 `memmove` 滑动窗口策略，低成本折中方案

### libdeflate → ✅ 必须，已完成

- **正题**: zlib 性能平庸；libdeflate 利用现代 CPU 指令集，解压速度 2-3 倍
- **反题**: 不支持流式解压，引入新外部依赖
- **合题**: 收益巨大（整体吞吐量可能翻倍），远超引入依赖的成本

### SIMD 指令集 → ❌ 暂不实施

- **正题**: 对海量文本数据逐字节处理，SIMD 理论加速比 16x/32x
- **反题**: FASTQ 是变长记录，加载/对齐数据的开销可能抵消计算加速；写 Intrinsics 代码极难阅读且需要多平台兼容
- **合题**: 当前瓶颈在 IO/解压而非计算，维护成本高。仅在 profiling 证明必要后再考虑

### Huge Pages → ❌ 不实施

- **正题**: 大 Buffer 频繁访问时 TLB Miss 导致性能下降
- **反题**: 需要操作系统配置支持；现代 CPU 硬件预取器已工作得很好
- **合题**: 低优先级，属于过度优化。除非 profiling 证实 TLB Miss 是瓶颈

### 异步日志 → ❌ 不实施

- **正题**: 磁盘 IO 慢速操作可能阻塞主线程
- **反题**: 默认 Info/Warn 级别每秒只有几行输出，同步日志开销几乎为零；异步日志在 Crash 时可能丢失关键日志
- **合题**: 保持默认同步，确保错误不丢失

### 动态插件系统 → ❌ 不实施

- **正题**: 用户可在不重新编译的情况下添加过滤逻辑
- **反题**: 动态库函数调用无法内联，在计算密集型循环中性能回退严重；99% 用户只需要标准功能
- **合题**: 反模式。更好的设计是丰富内置 Mutator 库

### 并行解析 → ❌ 暂不实施

- **理由**: TBB parallel_pipeline 已提供足够的并行度，并行解析需要复杂的记录边界检测，收益有限

---

## 综合评估

| 优化点 | 实施状态 | 性能影响 | 说明 |
| :--- | :--- | :--- | :--- |
| **libdeflate** | ✅ 已完成 | 显著提升 | 核心性能瓶颈，首要任务 |
| **内存池** | ✅ 已完成 | 中等提升 | 减少分配开销 |
| **SIMD** | ❌ 暂不实施 | 明显提升 | 维护成本高，当前瓶颈不在计算 |
| **Parser 鲁棒性** | 📋 待评估 | 轻微下降 (可控) | 可在后续版本考虑 |
| **其他** | ❌ 不实施 | — | Huge Pages、插件系统、异步日志等均不实施 |

**核心原则**: 简单优先，避免过度设计，保持代码可维护性。

---

## 后续可考虑的优化

在有明确 profiling 数据支持时可考虑：

1. **SIMD 质量检查** - 如果质量过滤成为瓶颈
2. **io_uring** - 如果 IO 成为明确瓶颈且目标平台支持
3. **内存预取** - 如果 cache miss 成为明确瓶颈

## 性能验证方法

```bash
# 对比有/无内存池的性能
time fqtools filter -i large.fq.gz -o out.fq.gz -t 8

# 使用 perf 分析热点
perf record -g fqtools filter -i large.fq.gz -o out.fq.gz -t 8
perf report
```

关注指标：吞吐量 (MB/s)、内存分配次数、CPU 利用率
