/**
 * @file fastq_reader.h
 * @brief FASTQ 文件读取器，支持 gzip 自动解压和批量零拷贝读取
 */

#pragma once

#include <cstddef>
#include <limits>
#include <memory>
#include <string>

#include "fastq_io.h"
#include "interfaces.h"

namespace fq::io {

struct FastqReaderOptions {
    size_t readChunkBytes = 1 * 1024 * 1024;  ///< 单次读取字节数
    size_t zlibBufferBytes = 128 * 1024;      ///< zlib 解压缓冲区大小
    size_t maxBufferBytes = 0;                ///< 最大缓冲区大小（0 = 无限制）
};

/// FASTQ 文件读取器（move-only），根据文件头自动检测 gzip
class FastqReader : public IReader {
public:
    using IReader::nextBatch;  // 引入基类无上限重载（否则被下方 override 名字隐藏）

    explicit FastqReader(const std::string& path);
    FastqReader(const std::string& path, const FastqReaderOptions& options);
    ~FastqReader() override;

    FastqReader(const FastqReader&) = delete;
    FastqReader& operator=(const FastqReader&) = delete;
    FastqReader(FastqReader&&) noexcept;
    FastqReader& operator=(FastqReader&&) noexcept;

    // 无默认参数：基类 IReader 以非虚单参重载提供无上限调用
    [[nodiscard]] auto nextBatch(FastqBatch& batch, size_t maxRecords) -> bool override;

    [[nodiscard]] auto isOpen() const -> bool;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fq::io
