/**
 * @file fastq_io.h
 * @brief FASTQ I/O 核心数据结构
 * @details 定义 FASTQ 记录和批次的核心数据结构，提供零拷贝访问接口。
 *
 * 本文件是 FASTQ I/O 模块的基础，包含：
 * - FastqRecord: 单条 FASTQ 记录的视图结构（零拷贝）
 * - FastqBatch: 批量 FASTQ 记录的容器
 *
 * @author LessUp
 * @date 2023-10-05
 * @version 1.0
 * @copyright (c) 2023 LessUp. All rights reserved.
 */

#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>
#include <string_view>
#include <vector>

namespace fq::io {

/**
 * @brief FASTQ 记录视图结构
 * @details 零拷贝结构，仅指向 FastqBatch 中的内存，不拥有数据。
 *
 * FASTQ 格式示例：
 * @code
 * @SEQ_ID COMMENT
 * GATCGATCGATC
 * +
 * IIIIIIIIIIII
 * @endcode
 *
 * @note 该结构不拥有内存，生命周期依赖于所属的 FastqBatch
 */
struct FastqRecord {
    std::string_view id;       ///< 序列 ID（不含 @ 前缀）
    std::string_view comment;  ///< ID 行的注释部分（可选）
    std::string_view seq;      ///< 序列内容（碱基）
    std::string_view qual;     ///< 质量值字符串
    std::string_view plus;     ///< '+' 行原文（含前导 +）

    /**
     * @brief 检查记录是否为空
     * @return 序列是否为空
     */
    [[nodiscard]] auto empty() const -> bool {
        return seq.empty();
    }

    /**
     * @brief 获取序列长度
     * @return 序列长度（碱基数）
     */
    [[nodiscard]] auto length() const -> size_t {
        return seq.size();
    }

    /**
     * @brief 校验记录完整性
     * @details 检查序列和质量值长度是否匹配
     * @return 记录是否有效
     */
    [[nodiscard]] auto validateLengths() const -> bool {
        return !seq.empty() && seq.size() == qual.size();
    }
};

/**
 * @brief FASTQ 批次数据容器
 * @details 拥有并管理一块连续内存，用于存储批量的 FASTQ 记录。
 *
 * 设计特点：
 * - 内存连续，缓存友好
 * - 支持内存复用（clear() 不释放内存）
 * - 支持迭代器访问
 *
 * @example
 * @code
 * fq::io::FastqBatch batch(4 * 1024 * 1024);  // 4MB 容量
 * // ... 填充数据 ...
 * for (const auto& record : batch) {
 *     std::cout << record.id << "\n";
 * }
 * batch.clear();  // 复用内存
 * @endcode
 */
class FastqBatch {
public:
    /**
     * @brief 构造批次（指定容量）
     * @param capacityBytes 缓冲区预分配容量（字节）
     */
    explicit FastqBatch(size_t capacityBytes = 4 * 1024 * 1024) {  // 默认 4MB
        buffer_.reserve(capacityBytes);
        records_.reserve(capacityBytes / 150);  // 粗略预估
    }

    /**
     * @brief 构造批次（指定容量和预期记录数）
     * @param capacityBytes 缓冲区预分配容量（字节）
     * @param expectedRecords 预期记录数量
     */
    FastqBatch(size_t capacityBytes, size_t expectedRecords) {
        buffer_.reserve(capacityBytes);
        records_.reserve(expectedRecords);
    }

    /**
     * @brief 清空数据（复用内存）
     * @details 清空记录但不释放内存，适用于循环使用场景
     */
    void clear() {
        buffer_.clear();
        records_.clear();
    }

    /// @name 迭代器支持
    /// @{
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
    /// @}

    /**
     * @brief 获取记录数量
     * @return 批次中的记录数
     */
    [[nodiscard]] auto size() const -> size_t {
        return records_.size();
    }

    /**
     * @brief 检查批次是否为空
     * @return 是否没有记录
     */
    [[nodiscard]] auto empty() const -> bool {
        return records_.empty();
    }

    /// @name 原始数据访问（供 Reader/Writer 使用）
    /// @{
    [[nodiscard]] auto buffer() -> std::vector<char>& {
        return buffer_;
    }
    [[nodiscard]] auto records() -> std::vector<FastqRecord>& {
        return records_;
    }
    [[nodiscard]] auto buffer() const -> const std::vector<char>& {
        return buffer_;
    }
    /// @}

    /**
     * @brief 将未处理完的碎片移动到缓冲区头部
     * @param validEndPos 有效数据的结束位置
     * @return 移动的字节数
     * @details 供 Reader 使用，处理跨批次的不完整记录
     */
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
};

}  // namespace fq::io
