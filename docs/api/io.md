# IO 模块 API

命名空间：`fq::io`

---

## FastqRecord

FASTQ 记录的零拷贝视图，使用 `std::string_view` 指向 `FastqBatch` 的连续内存。

### 字段

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | `std::string_view` | 记录标识符 |
| `sequence` | `std::string_view` | DNA 序列 |
| `quality` | `std::string_view` | 质量分数字符串 |
| `separator` | `std::string_view` | 分隔符行（通常为 `+`） |

### 关键方法

```cpp
auto averageQuality(int qualityEncoding = 33) const -> double;
auto length() const -> size_t;
auto gcContent() const -> double;
auto nRatio() const -> double;
```

---

## FastqBatch

批量存储多条 FASTQ 记录的容器，维护连续内存缓冲区。

### 关键方法

```cpp
auto records() const -> const std::vector<FastqRecord>&;
auto size() const -> size_t;
auto empty() const -> bool;
void clear();
void reserve(size_t count);
```

### 内存模型

```
FastqBatch
├── buffer_     连续内存块（存储原始文本）
└── records_    FastqRecord 数组（string_view 指向 buffer_）
```

---

## FastqReader

FASTQ 文件读取器，支持纯文本和 gzip 压缩格式。

### 构造与使用

```cpp
fq::io::FastqReader reader("input.fastq.gz");

fq::io::FastqBatch batch;
while (reader.nextBatch(batch, 10000)) {
    for (const auto& record : batch.records()) {
        // 处理每条记录
    }
}
```

### 关键方法

```cpp
explicit FastqReader(const std::string& path);
auto nextBatch(FastqBatch& batch, size_t batchSize = 10000) -> bool;
auto isOpen() const -> bool;
```

### 性能参数

| 参数 | 说明 |
|------|------|
| `readChunkBytes` | 每次读取的字节数 |
| `zlibBufferBytes` | zlib 解压缓冲区大小 |
| `maxBufferBytes` | 最大缓冲区限制 |

---

## FastqWriter

FASTQ 文件写入器，支持纯文本和 gzip 压缩输出。

### 构造与使用

```cpp
fq::io::FastqWriter writer("output.fastq.gz");

for (const auto& record : batch.records()) {
    writer.write(record);
}
```

### 性能参数

| 参数 | 说明 |
|------|------|
| `zlibBufferBytes` | 压缩缓冲区大小 |
| `outputBufferBytes` | 输出缓冲区大小 |

---

## FastqBatchPool

基于 `ObjectPool<FastqBatch>` 的对象池，减少 TBB pipeline 中的频繁分配。

```cpp
auto pool = fq::io::createFastqBatchPool(initialSize, maxSize);
auto batch = pool->acquire();  // 从池获取
// shared_ptr 析构时自动归还
```
