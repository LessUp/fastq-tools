#include "fqtools/io/fastq_writer.h"

#include "fqtools/error/error.h"

#include <libdeflate.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <vector>

#ifdef __linux__
#include <fcntl.h>  // posix_fadvise
#endif

namespace fq::io {

static auto endsWithGzSuffix(const std::string& path) -> bool {
    constexpr const char* kGz = ".gz";
    if (path.size() < 3) {
        return false;
    }
    return path.compare(path.size() - 3, 3, kGz) == 0;
}

struct FastqWriter::Impl {
    int fd = -1;
    std::string path;
    FastqWriterOptions options{};
    FastqWriterCompressionMode compression = FastqWriterCompressionMode::Auto;
    std::vector<char> buffer;
    
    struct libdeflate_compressor* compressor = nullptr;
    std::vector<char> compressedBuffer;
    
    std::uint64_t totalUncompressedBytes = 0;
    static constexpr size_t kBufferThreshold = 64 * 1024;

    // 记录已刷写到磁盘的文件偏移，用于 posix_fadvise DONTNEED
    off_t flushedOffset = 0;

    explicit Impl(const std::string& p, const FastqWriterOptions& opt) : path(p), options(opt) {
        if (options.compression == FastqWriterCompressionMode::Auto) {
            compression = endsWithGzSuffix(path) ? FastqWriterCompressionMode::Gzip
                                                : FastqWriterCompressionMode::None;
        } else {
            compression = options.compression;
        }

        // Open file with standard POSIX IO
        fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
             throw fq::error::IOError(path, errno);
        }
        
        buffer.reserve(options.outputBufferBytes);

        if (compression == FastqWriterCompressionMode::Gzip) {
            // Level 6 is default zlib level
            compressor = libdeflate_alloc_compressor(6);
            if (!compressor) {
                ::close(fd);
                throw fq::error::FastQException(
                    fq::error::ErrorCategory::Resource,
                    fq::error::ErrorSeverity::Critical,
                    "Failed to allocate libdeflate compressor");
            }

            // Ensure compressed buffer is large enough for worst case
            // libdeflate_gzip_compress_bound provides the upper bound
            compressedBuffer.resize(
                libdeflate_gzip_compress_bound(compressor, options.outputBufferBytes));
        }
    }

    ~Impl() {
        if (fd >= 0) {
            try {
                flush();
            } catch (...) {
                // Destructors must not throw.
            }
            ::close(fd);
            if (compressor) {
                libdeflate_free_compressor(compressor);
            }
        }
    }

    void flush() {
        if (fd >= 0 && !buffer.empty()) {
            const char* outPtr = nullptr;
            size_t outSize = 0;

            if (compression == FastqWriterCompressionMode::Gzip) {
                const size_t compressedSize = libdeflate_gzip_compress(
                    compressor, buffer.data(), buffer.size(), compressedBuffer.data(),
                    compressedBuffer.size());
                if (compressedSize == 0) {
                    throw fq::error::IOError(path, 0);
                }
                outPtr = compressedBuffer.data();
                outSize = compressedSize;
            } else {
                outPtr = buffer.data();
                outSize = buffer.size();
            }

            size_t totalWritten = 0;
            while (totalWritten < outSize) {
                const ssize_t written = ::write(
                    fd, outPtr + totalWritten, outSize - totalWritten);
                if (written < 0) {
                    if (errno == EINTR) {
                        continue;
                    }
                    throw fq::error::IOError(path, errno);
                }
                if (written == 0) {
                    throw fq::error::IOError(path, 0);
                }
                totalWritten += static_cast<size_t>(written);
            }

             buffer.clear();

#ifdef __linux__
            // 通知内核释放已写出的 page cache，减少大文件写出时的内存压力
            ::posix_fadvise(fd, flushedOffset, static_cast<off_t>(outSize),
                            POSIX_FADV_DONTNEED);
            flushedOffset += static_cast<off_t>(outSize);
#endif
        }
    }

    /// 批量追加：先计算总大小，一次 resize，然后用 memcpy 拼接
    void appendRecord(const FastqRecord& rec) {
        size_t needed = 1 + rec.id.size() + 1 +  // @ + ID + \n
            rec.seq.size() + 1 +                 // Seq + \n
            2 +                                  // +\n
            rec.qual.size() + 1;                 // Qual + \n

        if (!rec.comment.empty()) {
            needed += 1 + rec.comment.size();  // Space + Comment
        }

        totalUncompressedBytes += needed;

        // Flush if buffer full
        if (buffer.size() + needed > buffer.capacity()) {
            flush();

            if (needed > buffer.capacity()) {
                size_t newCap = std::max(buffer.capacity() * 2, needed + 4096);
                buffer.reserve(newCap);

                if (compression == FastqWriterCompressionMode::Gzip) {
                    compressedBuffer.resize(libdeflate_gzip_compress_bound(compressor, newCap));
                }
            }
        }

        // 一次 resize + memcpy 批量拼接，避免逐字符 push_back/insert 开销
        const size_t oldSize = buffer.size();
        buffer.resize(oldSize + needed);
        char* dst = buffer.data() + oldSize;

        *dst++ = '@';
        std::memcpy(dst, rec.id.data(), rec.id.size());
        dst += rec.id.size();

        if (!rec.comment.empty()) {
            *dst++ = ' ';
            std::memcpy(dst, rec.comment.data(), rec.comment.size());
            dst += rec.comment.size();
        }
        *dst++ = '\n';

        std::memcpy(dst, rec.seq.data(), rec.seq.size());
        dst += rec.seq.size();
        *dst++ = '\n';

        *dst++ = '+';
        *dst++ = '\n';

        std::memcpy(dst, rec.qual.data(), rec.qual.size());
        dst += rec.qual.size();
        *dst++ = '\n';
    }
};

FastqWriter::FastqWriter(const std::string& path) : FastqWriter(path, FastqWriterOptions{}) {}

FastqWriter::FastqWriter(const std::string& path, const FastqWriterOptions& options)
    : impl_(std::make_unique<Impl>(path, options)) {
    // Constructor logic verified in Impl
}

FastqWriter::~FastqWriter() = default;

FastqWriter::FastqWriter(FastqWriter&&) noexcept = default;
FastqWriter& FastqWriter::operator=(FastqWriter&&) noexcept = default;

bool FastqWriter::isOpen() const {
    return impl_ && impl_->fd >= 0;
}

void FastqWriter::write(const FastqBatch& batch) {
    for (const auto& rec : batch) {
        impl_->appendRecord(rec);
    }
}

void FastqWriter::write(const FastqRecord& record) {
    impl_->appendRecord(record);
}

auto FastqWriter::totalUncompressedBytes() const -> std::uint64_t {
    return impl_ ? impl_->totalUncompressedBytes : 0;
}

}  // namespace fq::io
