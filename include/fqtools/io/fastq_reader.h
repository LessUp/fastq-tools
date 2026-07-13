/**
 * @file fastq_reader.h
 * @brief FASTQ 文件读取器接口
 * @details 提供高性能的 FASTQ 文件读取功能，支持 gzip 压缩文件自动解压。
 *
 * @author LessUp
 * @date 2023-10-05
 * @version 1.0
 * @copyright (c) 2023 LessUp. All rights reserved.
 */

#pragma once

#include <cstddef>
#include <limits>
#include <memory>
#include <string>

#include "fastq_io.h"
#include "interfaces.h"

namespace fq::io {

/**
 * @brief FASTQ 读取器配置选项
 * @details 用于配置 FastqReader 的行为参数
 */
struct FastqReaderOptions {
    size_t readChunkBytes = 1 * 1024 * 1024;  ///< 单次读取的字节数（默认 1MB）
    size_t zlibBufferBytes = 128 * 1024;      ///< zlib 解压缓冲区大小（字节）
    size_t maxBufferBytes = 0;                ///< 最大缓冲区大小（0 表示无限制）
};

/**
 * @brief FASTQ 文件读取器
 * @details 高性能 FASTQ 文件读取器，支持以下特性：
 *   - 自动 gzip 解压（根据文件扩展名 .gz）
 *   - 批量读取优化
 *   - 零拷贝记录访问（FastqRecord 指向 Batch 内存）
 *   - 可配置的缓冲区大小
 *   - 移动语义支持
 *
 * @note 该类不可拷贝，仅支持移动
 *
 * @example
 * @code
 * fq::io::FastqReader reader("input.fq.gz");
 * fq::io::FastqBatch batch;
 * while (reader.nextBatch(batch)) {
 *     for (const auto& record : batch) {
 *         // 处理记录
 *     }
 *     batch.clear();
 * }
 * @endcode
 */
class FastqReader : public IReader {
public:
    /**
     * @brief 构造读取器（默认选项）
     * @param path 输入文件路径（支持 .gz 扩展名自动解压）
     */
    explicit FastqReader(const std::string& path);

    /**
     * @brief 构造读取器（自定义选项）
     * @param path 输入文件路径
     * @param options 读取器配置选项
     */
    FastqReader(const std::string& path, const FastqReaderOptions& options);
    ~FastqReader() override;

    // 禁止拷贝
    FastqReader(const FastqReader&) = delete;
    FastqReader& operator=(const FastqReader&) = delete;

    // 移动语义
    FastqReader(FastqReader&&) noexcept;
    FastqReader& operator=(FastqReader&&) noexcept;

    /**
     * @brief 读取下一批 FASTQ 记录（限制记录数）
     * @param batch 输出批次（会被清空后填充）
     * @param maxRecords 最大读取记录数
     * @return 是否成功读取到数据
     */
    [[nodiscard]] auto nextBatch(FastqBatch& batch,
                                 size_t maxRecords = std::numeric_limits<size_t>::max())
        -> bool override;

    /**
     * @brief 检查文件是否成功打开
     * @return 文件打开状态
     */
    [[nodiscard]] auto isOpen() const -> bool;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fq::io
