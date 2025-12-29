#include "fqtools/io/fastq_writer.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <vector>

#include <zlib.h>

namespace fq::io {

struct FastqWriter::Impl {
    gzFile file = nullptr;
    std::string path;
    FastqWriterOptions options{};
    std::vector<char> buffer;
    std::uint64_t totalUncompressedBytes = 0;
    static constexpr size_t kBufferThreshold = 64 * 1024;

    explicit Impl(const std::string& p, const FastqWriterOptions& opt) : path(p), options(opt) {
        file = gzopen(path.c_str(), "wb");
        if (file) {
            gzbuffer(file, static_cast<unsigned>(options.zlibBufferBytes));
        }
        buffer.reserve(options.outputBufferBytes);
    }

    ~Impl() {
        if (file) {
            flush();
            gzclose(file);
        }
    }

    void flush() {
        if (file && !buffer.empty()) {
            int written = gzwrite(file, buffer.data(), static_cast<unsigned>(buffer.size()));
            if (written <= 0) {
                // Handle error? For now just log/ignore in dtor, throw in normal flow
            }
            buffer.clear();
        }
    }

    void appendRecord(const FastqRecord& rec) {
        // Calculate size needed
        // @ID [comment]\nSeq\n+\nQual\n
        size_t needed = 1 + rec.id.size() + 1 +  // @ + ID + \n (min)
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
            // If single record is huge, reserve enough
            if (needed > buffer.capacity()) {
                buffer.reserve(std::max(buffer.capacity() * 2, needed + 4096));
            }
        }

        // Append @ID
        buffer.push_back('@');
        buffer.insert(buffer.end(), rec.id.begin(), rec.id.end());

        // Append Comment
        if (!rec.comment.empty()) {
            buffer.push_back(' ');
            buffer.insert(buffer.end(), rec.comment.begin(), rec.comment.end());
        }
        buffer.push_back('\n');

        // Append Seq
        buffer.insert(buffer.end(), rec.seq.begin(), rec.seq.end());
        buffer.push_back('\n');

        // Append +
        buffer.push_back('+');
        buffer.push_back('\n');

        // Append Qual
        buffer.insert(buffer.end(), rec.qual.begin(), rec.qual.end());
        buffer.push_back('\n');
    }
};

FastqWriter::FastqWriter(const std::string& path) : FastqWriter(path, FastqWriterOptions{}) {}

FastqWriter::FastqWriter(const std::string& path, const FastqWriterOptions& options)
    : impl_(std::make_unique<Impl>(path, options)) {
    if (!impl_->file) {
        throw std::runtime_error("Failed to open output file: " + path);
    }
}

FastqWriter::~FastqWriter() = default;

FastqWriter::FastqWriter(FastqWriter&&) noexcept = default;
FastqWriter& FastqWriter::operator=(FastqWriter&&) noexcept = default;

bool FastqWriter::isOpen() const {
    return impl_ && impl_->file != nullptr;
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
