/**
 * @file reader_interface.h
 * @brief FASTQ Reader 抽象接口
 * @details 定义 FASTQ 读取器的最小接口，用于依赖注入和测试隔离。
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
 * @brief FASTQ Reader 抽象接口
 * @details 定义 FASTQ 读取器的最小接口。
 *
 * 设计原则：
 * - 最小接口：仅包含 Pipeline 实际需要的方法
 * - 构造时不抛异常：具体实现可通过 isOpen() 检查状态
 * - 批量读取：支持零拷贝的批量数据访问
 *
 * @note 使用 IReader 前缀遵循项目接口命名约定
 */
class IReader {
public:
    virtual ~IReader() = default;

    /**
     * @brief 读取下一批 FASTQ 记录
     * @param batch 输出批次（会被清空后填充）
     * @return 是否成功读取到数据（false 表示 EOF）
     * @throw fq::error::IOError 读取错误
     * @throw fq::error::FormatError 格式错误
     */
    [[nodiscard]] virtual auto nextBatch(FastqBatch& batch) -> bool = 0;
};

/// @brief IReader 的工厂别名，便于测试 mock
using ReaderPtr = std::unique_ptr<IReader>;

}  // namespace fq::io
