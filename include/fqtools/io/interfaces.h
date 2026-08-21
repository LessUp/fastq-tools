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
 * @details 具体实现（FastqReader）在构造失败时抛 IOError；
 *          isOpen() 可用于 move-from 状态检查。
 */
class IReader {
public:
    virtual ~IReader() = default;

    /**
     * @brief 读取下一批 FASTQ 记录
     * @param batch 输出批次（会被清空后填充）
     * @param maxRecords 本批最多读取的记录数，必须 >= 1（传 0 抛 std::invalid_argument）
     * @return false 表示 EOF；格式或 I/O 错误抛异常，不静默转换为 EOF
     */
    [[nodiscard]] virtual auto nextBatch(FastqBatch& batch, size_t maxRecords) -> bool = 0;

    /// 无上限重载。虚函数不给默认参数（默认参数按静态类型解析，属经典陷阱），
    /// 以非虚重载转发代替
    [[nodiscard]] auto nextBatch(FastqBatch& batch) -> bool {
        return nextBatch(batch, std::numeric_limits<size_t>::max());
    }
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
    /// @note 异常中止路径不会调用 finish()：执行管线在处理抛出异常时直接展开，
    ///       writer 析构负责丢弃未发布内容。实现方不应依赖 finish() 区分
    ///       正常结束与异常中止（后者以析构收场）。
    virtual void finish() = 0;
};

}  // namespace fq::io
