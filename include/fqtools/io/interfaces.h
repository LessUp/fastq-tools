/**
 * @file interfaces.h
 * @brief I/O 抽象接口：IReader / IWriter，用于依赖注入和测试隔离
 */

#pragma once

#include "fqtools/io/fastq_io.h"

#include <cstddef>
#include <cstdint>
#include <limits>

namespace fq::io {

/**
 * @brief FASTQ Reader 抽象接口
 * @details 构造时不抛异常，具体实现可通过 isOpen() 检查状态。
 */
class IReader {
public:
    virtual ~IReader() = default;

    /**
     * @brief 读取下一批 FASTQ 记录
     * @param batch 输出批次（会被清空后填充）
     * @param maxRecords 本批最多读取的记录数
     * @return false 表示 EOF；格式或 I/O 错误抛异常，不静默转换为 EOF
     */
    [[nodiscard]] virtual auto nextBatch(FastqBatch& batch,
                                         size_t maxRecords = std::numeric_limits<size_t>::max())
        -> bool = 0;
};

/**
 * @brief FASTQ Writer 抽象接口
 * @details write() 只接受数据，finish() 报告 flush/压缩流关闭/发布错误。
 */
class IWriter {
public:
    virtual ~IWriter() = default;

    /// 写入一批记录，返回本批接受的未压缩 FASTQ 字节数
    virtual auto write(const FastqBatch& batch) -> std::uint64_t = 0;

    /// 显式完成协议：flush、压缩流关闭、输出发布。析构只做兜底清理。
    virtual void finish() = 0;
};

}  // namespace fq::io
