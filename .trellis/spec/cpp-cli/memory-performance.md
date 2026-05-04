# Memory & Performance Guidelines

> Memory management and performance constraints for FastQTools.

---

## Overview

FastQTools 强调零拷贝和高性能。所有代码必须遵循以下约束。

---

## Zero-Copy Principles

### 使用 string_view

```cpp
// FastqRecord 使用 string_view 避免复制
struct FastqRecord {
    std::string_view id;       // 指向原始数据
    std::string_view seq;      // 指向原始数据
    std::string_view qual;     // 指向原始数据
    std::string_view comment;  // 指向原始数据
};
```

### 生命周期安全

```cpp
// ✓ 安全：原始数据生命周期足够长
void processBatch(const FastqBatch& batch) {
    for (const auto& record : batch.records()) {
        // record.id 等指向 batch 内部数据
        // batch 生命周期覆盖整个函数
    }
}

// ❌ 危险：返回指向临时数据的 string_view
std::string_view getTemporaryId() {
    std::string temp = "@read1";
    return temp;  // 悬空视图！
}
```

---

## Batch Processing

```cpp
// 默认批次大小
constexpr size_t kDefaultBatchSize = 10000;

// 批处理接口
class FastqReader {
public:
    auto nextBatch(FastqBatch& batch) -> bool;
private:
    std::string buffer_;  // 批次数据存储
};
```

---

## Parallel Pipeline

```cpp
// 使用 TBB 并行流水线
#include <oneapi/tbb.h>

void processFile(const std::string& path, Predicate pred) {
    tbb::parallel_pipeline(
        /*max_number_of_live_token=*/16,
        // 读取阶段
        tbb::make_filter<void, FastqBatch>(
            tbb::filter_mode::serial_in_order,
            [&](tbb::flow_control& fc) -> FastqBatch {
                FastqBatch batch;
                if (!reader.nextBatch(batch)) {
                    fc.stop();
                }
                return batch;
            }
        )
        // 处理阶段
        | tbb::make_filter<FastqBatch, FastqBatch>(
            tbb::filter_mode::parallel,
            [&](FastqBatch batch) -> FastqBatch {
                // 并行处理
                return batch;
            }
        )
        // 写入阶段
        | tbb::make_filter<FastqBatch, void>(
            tbb::filter_mode::serial_in_order,
            [&](FastqBatch batch) {
                writer.writeBatch(batch);
            }
        )
    );
}
```

---

## Hot Path Constraints

```cpp
// ❌ 热路径中禁止串行瓶颈
void processRecord(const FastqRecord& record) {
    std::lock_guard lock(mutex_);  // 避免在热路径加锁
    // ...
}

// ❌ 热路径中禁止字符串复制
void processRecord(const FastqRecord& record) {
    std::string id = record.id;  // 避免不必要的复制
    // ...
}

// ❌ 热路径中禁止内存分配
void processRecord(const FastqRecord& record) {
    auto buffer = std::make_unique<char[]>(1024);  // 避免频繁分配
    // ...
}
```

---

## Memory Layout

```cpp
// 批次数据连续存储
class FastqBatch {
private:
    std::string buffer_;                    // 原始数据
    std::vector<FastqRecord> records_;      // 记录视图
};
```

---

## Performance Targets

| 指标 | 目标 |
|------|------|
| 吞吐量 | > 1M reads/sec |
| 内存占用 | < 100MB per 1M reads |
| 启动时间 | < 100ms |

---

## Logging Overhead

```cpp
// 使用异步日志（spdlog 默认异步）
fq::logging::debug("Processing record {}", record.id);

// 热路径避免详细日志
// ❌ 每条记录都记录
for (const auto& record : batch) {
    fq::logging::trace("Record: {}", record.id);  // 性能影响
}

// ✓ 批次级别日志
fq::logging::debug("Processed {} records", batch.size());
```

---

## Anti-Patterns

```cpp
// ❌ 破坏 string_view 生命周期
std::string_view getId() {
    return std::string("@read1");  // 返回临时对象的视图
}

// ❌ 不必要的字符串复制
void process(const FastqRecord& record) {
    std::string seq = record.seq;  // 复制了
    // 如果只需要读取，直接用 record.seq
}

// ❌ 串行化并行代码
tbb::parallel_for(0, 100, [&](int i) {
    std::lock_guard lock(mutex);  // 并行循环中的锁
    // ...
});
```

---

## Best Practices

1. **优先使用 string_view**：避免不必要的字符串复制
2. **批量处理**：使用 FastqBatch 而非单条记录
3. **并行流水线**：使用 TBB parallel_pipeline
4. **避免热路径瓶颈**：不在热路径加锁、分配、复制
5. **异步日志**：使用 spdlog 异步模式
