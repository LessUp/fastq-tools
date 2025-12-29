#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "fastq_io.h"

namespace fq::io {

struct FastqWriterOptions {
    size_t zlibBufferBytes = 128 * 1024;
    size_t outputBufferBytes = 128 * 1024;
};

class FastqWriter {
public:
    explicit FastqWriter(const std::string& path);
    FastqWriter(const std::string& path, const FastqWriterOptions& options);
    ~FastqWriter();

    // 禁止拷贝
    FastqWriter(const FastqWriter&) = delete;
    FastqWriter& operator=(const FastqWriter&) = delete;

    // 移动语义
    FastqWriter(FastqWriter&&) noexcept;
    FastqWriter& operator=(FastqWriter&&) noexcept;

    void write(const FastqBatch& batch);
    void write(const FastqRecord& record);
    bool isOpen() const;

    [[nodiscard]] auto totalUncompressedBytes() const -> std::uint64_t;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fq::io
