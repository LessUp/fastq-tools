# IO Module API

Namespace: `fq::io`

---

## FastqRecord

Zero-copy view of FASTQ record, using `std::string_view` pointing to continuous memory in `FastqBatch`.

### Fields

| Field | Type | Description |
|-------|------|-------------|
| `id` | `std::string_view` | Record identifier |
| `sequence` | `std::string_view` | DNA sequence |
| `quality` | `std::string_view` | Quality score string |
| `separator` | `std::string_view` | Separator line (usually `+`) |

### Key Methods

```cpp
auto averageQuality(int qualityEncoding = 33) const -> double;
auto length() const -> size_t;
auto gcContent() const -> double;
auto nRatio() const -> double;
```

---

## FastqBatch

Container for batch storage of multiple FASTQ records, maintaining continuous memory buffer.

### Key Methods

```cpp
auto records() const -> const std::vector<FastqRecord>&;
auto size() const -> size_t;
auto empty() const -> bool;
void clear();
void reserve(size_t count);
```

### Memory Model

```
FastqBatch
├── buffer_     Continuous memory block (stores raw text)
└── records_    FastqRecord array (string_view points to buffer_)
```

---

## FastqReader

FASTQ file reader, supports plain text and gzip compressed formats.

### Construction and Usage

```cpp
fq::io::FastqReader reader("input.fastq.gz");

fq::io::FastqBatch batch;
while (reader.nextBatch(batch, 10000)) {
    for (const auto& record : batch.records()) {
        // Process each record
    }
}
```

### Key Methods

```cpp
explicit FastqReader(const std::string& path);
auto nextBatch(FastqBatch& batch, size_t batchSize = 10000) -> bool;
auto isOpen() const -> bool;
```

### Performance Parameters

| Parameter | Description |
|-----------|-------------|
| `readChunkBytes` | Bytes to read each time |
| `zlibBufferBytes` | zlib decompression buffer size |
| `maxBufferBytes` | Maximum buffer limit |

---

## FastqWriter

FASTQ file writer, supports plain text and gzip compressed output.

### Construction and Usage

```cpp
fq::io::FastqWriter writer("output.fastq.gz");

for (const auto& record : batch.records()) {
    writer.write(record);
}
```

### Performance Parameters

| Parameter | Description |
|-----------|-------------|
| `zlibBufferBytes` | Compression buffer size |
| `outputBufferBytes` | Output buffer size |

---

## FastqBatchPool

Object pool based on `ObjectPool<FastqBatch>`, reducing frequent allocations in TBB pipeline.

```cpp
auto pool = fq::io::createFastqBatchPool(initialSize, maxSize);
auto batch = pool->acquire();  // Acquire from pool
// shared_ptr destructor automatically returns to pool
```
