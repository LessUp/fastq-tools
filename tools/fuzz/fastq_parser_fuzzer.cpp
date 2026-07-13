/**
 * @file fastq_parser_fuzzer.cpp
 * @brief 真实 FASTQ 解析器模糊测试
 *
 * 直接调用 fq::io::FastqReader::nextBatch 解析 fuzz 数据，
 * 覆盖指针算术、string_view 切片、\r\n 边界、跨批次 remainder、
 * gzip sniff、maxBufferBytes 限制等真实路径。
 *
 * 实现：将 fuzz 输入写入 tmpfile，构造 FastqReader 读取并迭代所有批次，
 * 访问每条记录字段以触发 string_view 越界读（ASan/MSan 可捕获）。
 *
 * 构建：cmake -DENABLE_FUZZING=ON -DCMAKE_CXX_COMPILER=clang++ -B build-fuzz
 * 运行：./build-fuzz/fuzzers/fastq_parser_fuzzer tools/fuzz/corpus/ -max_len=4096
 */

#include "fqtools/error/error.h"
#include "fqtools/io/fastq_reader.h"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>

#include <fcntl.h>
#include <unistd.h>

namespace {

/// @brief RAII 临时文件，析构时关闭 fd 并 unlink 路径
/// @note 不在构造时 unlink：FastqReader 需通过 path 再次 open 获取独立 fd
class TempFile {
public:
    TempFile() {
        const auto* tmpdir = std::getenv("TMPDIR");
        if (tmpdir == nullptr || tmpdir[0] == '\0') {
            tmpdir = "/tmp";
        }
        const auto tpl = std::filesystem::path(tmpdir) / "fq_fuzz_XXXXXX";
        std::string buf = tpl.string();
        fd_ = ::mkstemp(buf.data());
        if (fd_ >= 0) {
            path_ = buf;
        }
    }
    ~TempFile() {
        if (fd_ >= 0) {
            ::close(fd_);
        }
        if (!path_.empty()) {
            ::unlink(path_.c_str());
        }
    }
    TempFile(const TempFile&) = delete;
    auto operator=(const TempFile&) -> TempFile& = delete;
    TempFile(TempFile&&) = delete;
    auto operator=(TempFile&&) -> TempFile& = delete;

    [[nodiscard]] auto isOpen() const -> bool { return fd_ >= 0; }
    /// 路径在析构前有效（reader 会通过 path 再 open 独立 fd）
    [[nodiscard]] auto path() const -> const std::string& { return path_; }

    auto writeAll(const uint8_t* data, size_t size) -> bool {
        size_t off = 0;
        while (off < size) {
            const auto n = ::write(fd_, data + off, size - off);
            if (n < 0) {
                if (errno == EINTR) {
                    continue;
                }
                return false;
            }
            off += static_cast<size_t>(n);
        }
        return true;
    }

private:
    int fd_ = -1;
    std::string path_;
};

/// @brief 配置多样的 Reader 选项，覆盖不同缓冲区/限制路径
auto fuzzReaderOptions(uint8_t selector) -> fq::io::FastqReaderOptions {
    fq::io::FastqReaderOptions opts;
    switch (selector % 4) {
        case 0:
            // 默认（1MB chunk，无上限）
            break;
        case 1:
            // 极小 chunk，逼迫跨批次 remainder 路径
            opts.readChunkBytes = 16;
            opts.zlibBufferBytes = 128;
            break;
        case 2:
            // 极小 maxBufferBytes，触发 maxBufferBytes 守卫
            opts.readChunkBytes = 64;
            opts.maxBufferBytes = 128;
            break;
        case 3:
            // 大 chunk + 中等上限
            opts.readChunkBytes = 4096;
            opts.maxBufferBytes = 8192;
            break;
    }
    return opts;
}

/// @brief 迭代 reader 所有批次并访问每条记录字段（触发 string_view 越界读）
auto drainReader(fq::io::FastqReader& reader) -> void {
    fq::io::FastqBatch batch(64 * 1024);
    size_t totalRecords = 0;
    constexpr size_t kMaxRecordsPerRun = 10000;  // 防止恶意超大输入拖垮 fuzzer
    while (totalRecords < kMaxRecordsPerRun) {
        bool got = false;
        try {
            got = reader.nextBatch(batch);
        } catch (const fq::error::FastQException&) {
            // 解析错误是预期行为：fuzz 目标正是畸形输入
            return;
        } catch (const std::exception&) {
            return;
        }
        if (!got) {
            return;
        }
        for (const auto& rec : batch) {
            // 访问所有字段：string_view 越界会被 ASan/MSan 捕获
            volatile size_t idLen = rec.id.size();
            volatile size_t commentLen = rec.comment.size();
            volatile size_t seqLen = rec.seq.size();
            volatile size_t qualLen = rec.qual.size();
            volatile size_t plusLen = rec.plus.size();
            (void)idLen;
            (void)commentLen;
            (void)seqLen;
            (void)qualLen;
            (void)plusLen;

            // 访问最后一个字节（若存在）触发边界读
            if (!rec.seq.empty()) {
                volatile char lastBase = rec.seq.back();
                (void)lastBase;
            }
            if (!rec.qual.empty()) {
                volatile char lastQual = rec.qual.back();
                (void)lastQual;
            }

            // 不变式校验：解析成功的记录 seq/qual 必须等长（解析器已 validateLengths）
            // 若违反说明解析器有 bug，abort 让 fuzzer 报告
            if (rec.seq.size() != rec.qual.size()) {
                __builtin_trap();
            }

            ++totalRecords;
        }
        batch.clear();
    }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    // 限制输入大小，防止 OOM
    if (size > 1024 * 1024) {
        return 0;
    }
    if (size == 0) {
        return 0;
    }

    // 用首字节作为 reader 选项 selector，剩余字节作为文件内容
    const uint8_t selector = data[0];
    const uint8_t* payload = data + 1;
    const size_t payloadSize = size - 1;

    TempFile tmp;
    if (!tmp.isOpen()) {
        return 0;
    }
    if (!tmp.writeAll(payload, payloadSize)) {
        return 0;
    }

    const auto opts = fuzzReaderOptions(selector);
    try {
        fq::io::FastqReader reader(tmp.path(), opts);
        if (!reader.isOpen()) {
            return 0;
        }
        drainReader(reader);
    } catch (const fq::error::FastQException&) {
        // 预期
    } catch (const std::exception&) {
        // 预期
    }

    return 0;
}
