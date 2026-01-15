#include "fqtools/io/fastq_writer.h"

#include <libdeflate.h>
#include <fcntl.h>
#include <unistd.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <vector>

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
             throw std::runtime_error("Failed to open output file: " + path);
        }
        
        buffer.reserve(options.outputBufferBytes);

        if (compression == FastqWriterCompressionMode::Gzip) {
            // Level 6 is default zlib level
            compressor = libdeflate_alloc_compressor(6);
            if (!compressor) {
                ::close(fd);
                throw std::runtime_error("Failed to allocate libdeflate compressor");
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
                    throw std::runtime_error("Failed to compress output buffer");
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
                    throw std::runtime_error("Failed to write output file: " + path);
                }
                if (written == 0) {
                    throw std::runtime_error("Failed to write output file: " + path);
                }
                totalWritten += static_cast<size_t>(written);
            }

             buffer.clear();
        }
    }

    void appendRecord(const FastqRecord& rec) {
        // Calculate size needed
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
            
            // If single record is huge, resize buffer and compressed buffer
            if (needed > buffer.capacity()) {
                size_t newCap = std::max(buffer.capacity() * 2, needed + 4096);
                buffer.reserve(newCap);

                if (compression == FastqWriterCompressionMode::Gzip) {
                    compressedBuffer.resize(libdeflate_gzip_compress_bound(compressor, newCap));
                }
            }
        }

        // Append logic
        buffer.push_back('@');
        buffer.insert(buffer.end(), rec.id.begin(), rec.id.end());

        if (!rec.comment.empty()) {
            buffer.push_back(' ');
            buffer.insert(buffer.end(), rec.comment.begin(), rec.comment.end());
        }
        buffer.push_back('\n');

        buffer.insert(buffer.end(), rec.seq.begin(), rec.seq.end());
        buffer.push_back('\n');

        buffer.push_back('+');
        buffer.push_back('\n');

        buffer.insert(buffer.end(), rec.qual.begin(), rec.qual.end());
        buffer.push_back('\n');
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
