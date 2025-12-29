#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <string_view>
#include <vector>

namespace fq::io {

/**
 * @brief FastqRecord 视图
 * @details 零拷贝结构，仅指向 FastqBatch 中的内存。
 */
struct FastqRecord {
    std::string_view id;       // 序列 ID (不含 @)
    std::string_view comment;  // ID 后的注释
    std::string_view seq;      // 序列内容
    std::string_view qual;     // 质量值内容
    std::string_view plus;     // + 行内容（通常为空或重复ID）

    // 检查记录是否完整/有效
    [[nodiscard]] auto empty() const -> bool {
        return seq.empty();
    }
    [[nodiscard]] auto length() const -> size_t {
        return seq.size();
    }

    // 基础校验：长度是否匹配
    [[nodiscard]] auto validateLengths() const -> bool {
        return !seq.empty() && seq.size() == qual.size();
    }
};

/**
 * @brief FastqBatch 数据容器
 * @details 拥有并管理一块连续内存，用于存储批量的 FASTQ 记录。
 */
class FastqBatch {
public:
    explicit FastqBatch(size_t capacityBytes = 4 * 1024 * 1024) {  // 默认 4MB
        buffer_.reserve(capacityBytes);
        records_.reserve(capacityBytes / 150);  // 粗略预估
    }

    FastqBatch(size_t capacityBytes, size_t expectedRecords) {
        buffer_.reserve(capacityBytes);
        records_.reserve(expectedRecords);
    }

    // 清空数据（复用内存）
    void clear() {
        buffer_.clear();
        records_.clear();
        remainderOffset_ = 0;
    }

    // 记录访问
    [[nodiscard]] auto begin() const {
        return records_.begin();
    }
    [[nodiscard]] auto end() const {
        return records_.end();
    }
    [[nodiscard]] auto begin() {
        return records_.begin();
    }
    [[nodiscard]] auto end() {
        return records_.end();
    }

    [[nodiscard]] auto size() const -> size_t {
        return records_.size();
    }
    [[nodiscard]] auto empty() const -> bool {
        return records_.empty();
    }

    // 原始数据操作（供 Reader 使用）
    [[nodiscard]] auto buffer() -> std::vector<char>& {
        return buffer_;
    }
    [[nodiscard]] auto records() -> std::vector<FastqRecord>& {
        return records_;
    }
    [[nodiscard]] auto buffer() const -> const std::vector<char>& {
        return buffer_;
    }

    // 将未处理完的碎片移动到 Buffer 头部（供 Reader 使用）
    // 返回移动的字节数
    auto moveRemainderToStart(size_t validEndPos) -> size_t {
        if (validEndPos >= buffer_.size()) {
            buffer_.clear();
            return 0;
        }
        size_t remainderLen = buffer_.size() - validEndPos;
        // 使用 memmove 语义，vector::erase/insert 或 copy 可能涉及分配
        // 这里直接在 vector 内部操作最快
        if (remainderLen > 0) {
            std::memmove(buffer_.data(), buffer_.data() + validEndPos, remainderLen);
            buffer_.resize(remainderLen);
        } else {
            buffer_.clear();
        }
        return remainderLen;
    }

private:
    std::vector<char> buffer_;
    std::vector<FastqRecord> records_;
    size_t remainderOffset_ = 0;
};

}  // namespace fq::io
