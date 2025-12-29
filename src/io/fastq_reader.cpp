#include "fqtools/io/fastq_reader.h"

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>

#include <zlib.h>

namespace fq::io {

struct FastqReader::Impl {
    gzFile file = nullptr;
    std::string path;
    bool isEofReached = false;
    FastqReaderOptions options{};
    std::vector<char> remainder;

    explicit Impl(const std::string& p, const FastqReaderOptions& opt) : path(p), options(opt) {
        file = gzopen(path.c_str(), "r");
        if (file) {
            gzbuffer(file, static_cast<unsigned>(options.zlibBufferBytes));
        }
    }

    ~Impl() {
        if (file)
            gzclose(file);
    }

    static auto findEol(const char* ptr, const char* end) -> const char* {
        return static_cast<const char*>(std::memchr(ptr, '\n', static_cast<size_t>(end - ptr)));
    }
};

FastqReader::FastqReader(const std::string& path) : FastqReader(path, FastqReaderOptions{}) {}

FastqReader::FastqReader(const std::string& path, const FastqReaderOptions& options)
    : impl_(std::make_unique<Impl>(path, options)) {}

FastqReader::~FastqReader() = default;

FastqReader::FastqReader(FastqReader&&) noexcept = default;
FastqReader& FastqReader::operator=(FastqReader&&) noexcept = default;

auto FastqReader::isOpen() const -> bool {
    return impl_->file != nullptr;
}

auto FastqReader::nextBatch(FastqBatch& batch) -> bool {
    return nextBatch(batch, std::numeric_limits<size_t>::max());
}

auto FastqReader::nextBatch(FastqBatch& batch, size_t maxRecords) -> bool {
    if (!impl_->file) {
        return false;
    }

    batch.records().clear();
    batch.buffer().clear();

    if (!impl_->remainder.empty()) {
        batch.buffer().insert(
            batch.buffer().end(), impl_->remainder.begin(), impl_->remainder.end());
        impl_->remainder.clear();
    }

    if (batch.buffer().empty() && impl_->isEofReached) {
        return false;
    }

    while (true) {
        if (!impl_->isEofReached) {
            const size_t chunk = std::max<size_t>(1, impl_->options.readChunkBytes);
            const bool unlimited = (maxRecords == std::numeric_limits<size_t>::max());
            constexpr size_t bytesPerRecordEst = 512;
            const size_t maxBuf = impl_->options.maxBufferBytes;

            size_t targetBytes = 0;
            if (unlimited) {
                targetBytes = batch.buffer().size() + chunk;
            } else {
                const size_t want = maxRecords * bytesPerRecordEst;
                targetBytes = std::max(batch.buffer().size(), want);
            }

            if (maxBuf > 0) {
                targetBytes = std::min(targetBytes, maxBuf);
            }

            while (!impl_->isEofReached && batch.buffer().size() < targetBytes) {
                const auto kCurrentSize = batch.buffer().size();
                if (maxBuf > 0 && kCurrentSize >= maxBuf) {
                    break;
                }

                size_t toRead = chunk;
                if (maxBuf > 0) {
                    const auto kRemaining = maxBuf - kCurrentSize;
                    if (kRemaining == 0) {
                        break;
                    }
                    toRead = std::min(toRead, kRemaining);
                }

                if (batch.buffer().capacity() < kCurrentSize + chunk) {
                    const auto kNewCap =
                        std::max(batch.buffer().capacity() * 2, kCurrentSize + chunk);
                    batch.buffer().reserve(kNewCap);
                }

                batch.buffer().resize(kCurrentSize + toRead);
                const auto kBytesRead = gzread(impl_->file,
                                               batch.buffer().data() + kCurrentSize,
                                               static_cast<unsigned>(toRead));
                if (kBytesRead < 0) {
                    int err = 0;
                    const char* msg = gzerror(impl_->file, &err);
                    throw std::runtime_error(std::string("Gzip read error: ") +
                                             (msg != nullptr ? msg : "unknown"));
                }
                batch.buffer().resize(kCurrentSize + static_cast<size_t>(kBytesRead));
                if (kBytesRead == 0) {
                    impl_->isEofReached = true;
                }
            }
        }

        if (batch.buffer().empty()) {
            return false;
        }

        const char* data = batch.buffer().data();
        const char* end = data + batch.buffer().size();
        const char* ptr = data;
        const char* lastValidPtr = ptr;

        while (ptr < end && batch.records().size() < maxRecords) {
            while (ptr < end && (*ptr == '\n' || *ptr == '\r')) {
                ++ptr;
            }
            if (ptr >= end) {
                break;
            }

            if (*ptr != '@') {
                break;
            }

            const char* line1End = Impl::findEol(ptr, end);
            if (line1End == nullptr) {
                break;
            }

            const char* line2Start = line1End + 1;
            const char* line2End = Impl::findEol(line2Start, end);
            if (line2End == nullptr) {
                break;
            }

            const char* line3Start = line2End + 1;
            if (line3Start >= end || *line3Start != '+') {
                break;
            }
            const char* line3End = Impl::findEol(line3Start, end);
            if (line3End == nullptr) {
                break;
            }

            const char* line4Start = line3End + 1;
            const char* line4End = Impl::findEol(line4Start, end);
            if (line4End == nullptr) {
                if (impl_->isEofReached) {
                    line4End = end;
                } else {
                    break;
                }
            }

            FastqRecord rec;

            const auto kIdLen = static_cast<size_t>(line1End - ptr);
            size_t idLen = kIdLen;
            if (idLen > 0 && ptr[idLen - 1] == '\r') {
                --idLen;
            }
            const std::string_view kFullIdLine(ptr + 1, idLen > 0 ? (idLen - 1) : 0);
            const size_t kSpacePos = kFullIdLine.find_first_of(" \t");
            if (kSpacePos != std::string_view::npos) {
                rec.id = kFullIdLine.substr(0, kSpacePos);
                rec.comment = kFullIdLine.substr(kSpacePos + 1);
            } else {
                rec.id = kFullIdLine;
            }

            const auto kSeqLen = static_cast<size_t>(line2End - line2Start);
            size_t seqLen = kSeqLen;
            if (seqLen > 0 && line2Start[seqLen - 1] == '\r') {
                --seqLen;
            }
            rec.seq = std::string_view(line2Start, seqLen);

            const auto kQualLen = static_cast<size_t>(line4End - line4Start);
            size_t qualLen = kQualLen;
            if (qualLen > 0 && line4Start[qualLen - 1] == '\r') {
                --qualLen;
            }
            rec.qual = std::string_view(line4Start, qualLen);

            batch.records().push_back(rec);

            ptr = line4End;
            if (ptr < end && *ptr == '\n') {
                ++ptr;
            }
            lastValidPtr = ptr;
        }

        if (!batch.records().empty()) {
            const auto kConsumed = static_cast<size_t>(lastValidPtr - data);
            if (kConsumed < static_cast<size_t>(end - data)) {
                impl_->remainder.assign(batch.buffer().begin() +
                                             static_cast<std::ptrdiff_t>(kConsumed),
                                         batch.buffer().end());
                batch.buffer().resize(kConsumed);
            }
            return true;
        }

        if (impl_->isEofReached) {
            batch.buffer().clear();
            return false;
        }

        if (impl_->options.maxBufferBytes > 0 &&
            batch.buffer().size() >= impl_->options.maxBufferBytes) {
            throw std::runtime_error(
                "FastqReader reached maxBufferBytes without parsing a complete record; increase "
                "batchCapacityBytes/maxBufferBytes");
        }
    }
}

}  // namespace fq::io
