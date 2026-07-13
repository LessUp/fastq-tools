/**
 * @file interfaces.h
 * @brief FASTQ I/O 抽象接口集合
 * @details 定义 FASTQ 读取器和写入器的最小接口，用于依赖注入和测试隔离。
 */

#pragma once

#include "fqtools/io/fastq_io.h"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>

namespace fq::io {

/**
 * @brief FASTQ Reader 抽象接口
 * @details 定义 FASTQ 读取器的最小接口。
 *
 * 设计原则：
 * - 最小接口：仅包含 Pipeline 实际需要的方法
 * - 构造时不抛异常：具体实现可通过 isOpen() 检查状态
 * - 批量读取：支持零拷贝的批量数据访问
 */
class IReader {
public:
    virtual ~IReader() = default;

    /**
     * @brief 读取下一批 FASTQ 记录
     * @param batch 输出批次（会被清空后填充）
     * @param maxRecords 本批最多读取的记录数
     * @return 是否成功读取到数据（false 表示 EOF）
     * @throw fq::error::IOError 读取错误
     * @throw fq::error::FormatError 格式错误
     */
    [[nodiscard]] virtual auto nextBatch(
        FastqBatch& batch, size_t maxRecords = std::numeric_limits<size_t>::max()) -> bool = 0;
};

/// @brief IReader 的工厂别名，便于测试 mock
using ReaderPtr = std::unique_ptr<IReader>;

/**
 * @brief FASTQ Writer 抽象接口
 * @details 定义 FASTQ 写入器的最小接口。
 *
 * 设计原则：
 * - 最小接口：仅包含 Pipeline 实际需要的方法
 * - 批量写入：支持高效的批量数据输出
 */
class IWriter {
public:
    virtual ~IWriter() = default;

    /**
     * @brief 写入一批 FASTQ 记录
     * @param batch 要写入的记录批次
     * @return 本批提交的未压缩 FASTQ 字节数
     * @throw fq::error::IOError 写入错误
     */
    virtual auto write(const FastqBatch& batch) -> std::uint64_t = 0;
};

/// @brief IWriter 的工厂别名，便于测试 mock
using WriterPtr = std::unique_ptr<IWriter>;

}  // namespace fq::io
