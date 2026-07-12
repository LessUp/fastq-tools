/**
 * @file fastq_writer.h
 * @brief FASTQ 文件写入器接口
 * @details 提供高性能的 FASTQ 文件写入功能，支持 gzip 压缩输出。
 *
 * @author LessUp
 * @date 2023-10-05
 * @version 1.0
 * @copyright (c) 2023 LessUp. All rights reserved.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "fastq_io.h"
#include "interfaces.h"

namespace fq::io {

/**
 * @brief 压缩模式枚举
 * @details 定义 FASTQ 输出文件的压缩方式
 */
enum class FastqWriterCompressionMode : std::uint8_t {
    Auto,  ///< 自动检测（根据文件扩展名）
    Gzip,  ///< 强制 gzip 压缩
    None,  ///< 不压缩
};

/**
 * @brief FASTQ 写入器配置选项
 * @details 用于配置 FastqWriter 的行为参数
 */
struct FastqWriterOptions {
    size_t zlibBufferBytes = static_cast<size_t>(128) * 1024;  ///< zlib 压缩缓冲区大小（字节）
    size_t outputBufferBytes = static_cast<size_t>(128) * 1024;  ///< 输出缓冲区大小（字节）
    FastqWriterCompressionMode compression = FastqWriterCompressionMode::Auto;  ///< 压缩模式
};

/**
 * @brief FASTQ 文件写入器
 * @details 高性能 FASTQ 文件写入器，支持以下特性：
 *   - 自动 gzip 压缩（根据文件扩展名 .gz）
 *   - 批量写入优化
 *   - 可配置的缓冲区大小
 *   - 移动语义支持
 *
 * @note 该类不可拷贝，仅支持移动
 *
 * @example
 * @code
 * fq::io::FastqWriter writer("output.fq.gz");
 * writer.write(batch);
 * @endcode
 */
class FastqWriter : public IWriter {
public:
    /**
     * @brief 构造写入器（默认选项）
     * @param path 输出文件路径（支持 .gz 扩展名自动压缩）
     */
    explicit FastqWriter(const std::string& path);

    /**
     * @brief 构造写入器（自定义选项）
     * @param path 输出文件路径
     * @param options 写入器配置选项
     */
    FastqWriter(const std::string& path, const FastqWriterOptions& options);

    ~FastqWriter() override;

    // 禁止拷贝
    FastqWriter(const FastqWriter&) = delete;
    FastqWriter& operator=(const FastqWriter&) = delete;

    // 移动语义
    FastqWriter(FastqWriter&&) noexcept;
    FastqWriter& operator=(FastqWriter&&) noexcept;

    /**
     * @brief 写入一批 FASTQ 记录
     * @param batch 要写入的记录批次
     */
    void write(const FastqBatch& batch) override;

    /**
     * @brief 写入单条 FASTQ 记录
     * @param record 要写入的单条记录
     * @note 此方法不在 IWriter 接口中，为具体类额外提供
     */
    void write(const FastqRecord& record);

    /**
     * @brief 检查文件是否成功打开
     * @return 文件打开状态
     */
    [[nodiscard]] auto isOpen() const -> bool;

    /**
     * @brief 获取已写入的未压缩字节数
     * @return 未压缩数据的总字节数
     */
    [[nodiscard]] auto totalUncompressedBytes() const -> std::uint64_t;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fq::io
