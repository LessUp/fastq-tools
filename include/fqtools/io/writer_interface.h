/**
 * @file writer_interface.h
 * @brief FASTQ Writer 抽象接口
 * @details 定义 FASTQ 写入器的最小接口，用于依赖注入和测试隔离。
 *
 * @author FastQTools Team
 * @date 2026
 * @version 1.0
 * @copyright Copyright (c) 2026 FastQTools
 * @license MIT License
 */

#pragma once

#include "fqtools/io/fastq_io.h"

#include <memory>

namespace fq::io {

/**
 * @brief FASTQ Writer 抽象接口
 * @details 定义 FASTQ 写入器的最小接口。
 *
 * 设计原则：
 * - 最小接口：仅包含 Pipeline 实际需要的方法
 * - 批量写入：支持高效的批量数据输出
 *
 * @note 使用 IWriter 前缀遵循项目接口命名约定
 */
class IWriter {
public:
    virtual ~IWriter() = default;

    /**
     * @brief 写入一批 FASTQ 记录
     * @param batch 要写入的记录批次
     * @throw fq::error::IOError 写入错误
     */
    virtual void write(const FastqBatch& batch) = 0;
};

/// @brief IWriter 的工厂别名，便于测试 mock
using WriterPtr = std::unique_ptr<IWriter>;

}  // namespace fq::io
