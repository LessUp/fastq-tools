#pragma once

#include <cstddef>
#include <memory>
#include <string>

#include "fastq_io.h"

namespace fq::io {

struct FastqReaderOptions {
    size_t readChunkBytes = 1 * 1024 * 1024;
    size_t zlibBufferBytes = 128 * 1024;
    size_t maxBufferBytes = 0;
};

class FastqReader {
public:
    explicit FastqReader(const std::string& path);
    FastqReader(const std::string& path, const FastqReaderOptions& options);
    ~FastqReader();

    // 禁止拷贝
    FastqReader(const FastqReader&) = delete;
    FastqReader& operator=(const FastqReader&) = delete;

    // 移动语义
    FastqReader(FastqReader&&) noexcept;
    FastqReader& operator=(FastqReader&&) noexcept;

    [[nodiscard]] auto nextBatch(FastqBatch& batch) -> bool;

    [[nodiscard]] auto nextBatch(FastqBatch& batch, size_t maxRecords) -> bool;

    /**
     * @brief 检查文件是否成功打开
     */
    [[nodiscard]] auto isOpen() const -> bool;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fq::io
