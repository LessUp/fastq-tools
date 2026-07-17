#include "fqtools/io/fastq_writer.h"

#include "fqtools/error/error.h"
#include "fqtools/logging.h"

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <zlib.h>

#ifdef __linux__
#endif

namespace fq::io {

namespace {

auto makeTemporaryPath(const std::filesystem::path& target) -> std::filesystem::path {
    static std::atomic<std::uint64_t> counter{0};
    const auto parent =
        target.parent_path().empty() ? std::filesystem::path(".") : target.parent_path();
    const auto filename = target.filename().string();
    const auto processId = static_cast<unsigned long long>(::getpid());
    for (;;) {
        const auto suffix = ".tmp-" + std::to_string(processId) + "-" +
            std::to_string(counter.fetch_add(1, std::memory_order_relaxed));
        const auto candidate = parent / (filename + suffix);
        const int marker = ::open(candidate.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (marker >= 0) {
            ::close(marker);
            return candidate;
        }
        if (errno != EEXIST) {
            throw fq::error::IOError(candidate.string(), errno);
        }
    }
}

}  // namespace

static auto endsWithGzSuffix(const std::string& path) -> bool {
    constexpr const char* kGz = ".gz";
    if (path.size() < 3) {
        return false;
    }
    return path.compare(path.size() - 3, 3, kGz) == 0;
}

struct FastqWriter::Impl {
    int fd = -1;
    gzFile gzfile = nullptr;
    std::string path;
    std::filesystem::path temporaryPath;
    FastqWriterOptions options{};
    FastqWriterCompressionMode compression = FastqWriterCompressionMode::Auto;
    std::vector<char> buffer;

    std::atomic<std::uint64_t> totalUncompressedBytes{0};
    bool finished = false;
    bool finishFailed = false;

    // 记录已刷写到磁盘的文件偏移，用于 posix_fadvise DONTNEED
    off_t flushedOffset = 0;

    explicit Impl(std::string p, const FastqWriterOptions& opt) : path(std::move(p)), options(opt) {
        temporaryPath = makeTemporaryPath(path);

        try {
            if (options.compression == FastqWriterCompressionMode::Auto) {
                compression = endsWithGzSuffix(path) ? FastqWriterCompressionMode::Gzip
                                                     : FastqWriterCompressionMode::None;
            } else {
                compression = options.compression;
            }

            if (compression == FastqWriterCompressionMode::Gzip) {
                if (options.compressionLevel < 1 || options.compressionLevel > 9) {
                    throw fq::error::ConfigurationError(
                        "gzip compression level must be between 1 and 9");
                }
                // 使用 zlib gz API 写入 gzip 文件，默认压缩级别 6。
                const std::string mode = "wb" + std::to_string(options.compressionLevel);
                gzfile = gzopen(temporaryPath.c_str(), mode.c_str());
                if (!gzfile) {
                    throw fq::error::IOError(temporaryPath.string(), errno);
                }
                gzbuffer(gzfile, static_cast<unsigned>(options.outputBufferBytes));
            } else {
                fd = ::open(temporaryPath.c_str(), O_WRONLY | O_TRUNC, 0600);
                if (fd < 0) {
                    throw fq::error::IOError(temporaryPath.string(), errno);
                }
            }

            buffer.reserve(options.outputBufferBytes);
        } catch (...) {
            std::error_code error;
            std::filesystem::remove(temporaryPath, error);
            throw;
        }
    }

    ~Impl() {
        if (!finished && !finishFailed) {
            try {
                finish();
            } catch (const std::exception& e) {
                // 析构只做兜底；显式 finish() 的错误已由调用方获得。
                fq::logging::error("FastqWriter finish failed on close: {}", e.what());
                cleanupTemporaryFile();
            }
        } else if (!finished) {
            cleanupTemporaryFile();
        }
    }

    void cleanupTemporaryFile() noexcept {
        if (fd >= 0) {
            ::close(fd);
            fd = -1;
        }
        if (gzfile) {
            gzclose(gzfile);
            gzfile = nullptr;
        }
        std::error_code error;
        std::filesystem::remove(temporaryPath, error);
    }

    void finish() {
        if (finished) {
            return;
        }

        try {
            flush();

            if (fd >= 0) {
                if (::close(fd) != 0) {
                    const int error = errno;
                    fd = -1;
                    throw fq::error::IOError(path, error);
                }
                fd = -1;
            }

            if (gzfile) {
                const int closeResult = gzclose(gzfile);
                gzfile = nullptr;
                if (closeResult != Z_OK) {
                    throw fq::error::FastQException(fq::error::ErrorCategory::IO,
                                                    fq::error::ErrorSeverity::Critical,
                                                    "gzip close failed");
                }
            }

            std::error_code error;
            std::filesystem::rename(temporaryPath, path, error);
            if (error) {
                throw fq::error::IOError(path, error.value());
            }
            finished = true;
        } catch (...) {
            finishFailed = true;
            cleanupTemporaryFile();
            throw;
        }
    }

    void flush() {
        if (buffer.empty()) {
            return;
        }

        if (compression == FastqWriterCompressionMode::Gzip) {
            // gzwrite 返回写入的未压缩字节数（0 表示错误）
            const auto toWrite = static_cast<unsigned>(buffer.size());
            int written = gzwrite(gzfile, buffer.data(), toWrite);
            if (written == 0 || static_cast<unsigned>(written) != toWrite) {
                int err = 0;
                const char* msg = gzerror(gzfile, &err);
                throw fq::error::FastQException(fq::error::ErrorCategory::IO,
                                                fq::error::ErrorSeverity::Critical,
                                                msg ? msg : "gzwrite failed");
            }
        } else {
            const char* outPtr = buffer.data();
            size_t outSize = buffer.size();
            size_t totalWritten = 0;
            while (totalWritten < outSize) {
                const ssize_t written = ::write(fd, outPtr + totalWritten, outSize - totalWritten);
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

#ifdef __linux__
            // 通知内核释放已写出的 page cache，减少大文件写出时的内存压力
            ::posix_fadvise(fd, flushedOffset, static_cast<off_t>(outSize), POSIX_FADV_DONTNEED);
            flushedOffset += static_cast<off_t>(outSize);
#endif
        }

        buffer.clear();
    }

    /// 批量追加：先计算总大小，一次 resize，然后用 memcpy 拼接
    void appendRecord(const FastqRecord& rec) {
        const std::string_view plusLine = rec.plus.empty() ? std::string_view("+") : rec.plus;
        size_t needed = 1 + rec.id.size() + 1 +  // @ + ID + \n
            rec.seq.size() + 1 +                 // Seq + \n
            plusLine.size() + 1 +                // Plus + \n
            rec.qual.size() + 1;                 // Qual + \n

        if (!rec.comment.empty()) {
            needed += 1 + rec.comment.size();  // Space + Comment
        }

        totalUncompressedBytes.fetch_add(needed, std::memory_order_relaxed);

        // Flush if buffer full
        if (buffer.size() + needed > buffer.capacity()) {
            flush();

            if (needed > buffer.capacity()) {
                size_t newCap = std::max(buffer.capacity() * 2, needed + 4096);
                buffer.reserve(newCap);
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

        std::memcpy(dst, plusLine.data(), plusLine.size());
        dst += plusLine.size();
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
    return impl_ && (impl_->fd >= 0 || impl_->gzfile != nullptr);
}

auto FastqWriter::write(const FastqBatch& batch) -> std::uint64_t {
    if (!impl_ || impl_->finished) {
        throw fq::error::ConfigurationError("FastqWriter cannot write after finish");
    }
    const auto before = totalUncompressedBytes();
    for (const auto& rec : batch) {
        impl_->appendRecord(rec);
    }
    return totalUncompressedBytes() - before;
}

void FastqWriter::write(const FastqRecord& record) {
    if (!impl_ || impl_->finished) {
        throw fq::error::ConfigurationError("FastqWriter cannot write after finish");
    }
    impl_->appendRecord(record);
}

void FastqWriter::finish() {
    if (!impl_) {
        throw fq::error::ConfigurationError("FastqWriter is not initialized");
    }
    impl_->finish();
}

auto FastqWriter::totalUncompressedBytes() const -> std::uint64_t {
    return impl_ ? impl_->totalUncompressedBytes.load(std::memory_order_relaxed) : 0;
}

}  // namespace fq::io
