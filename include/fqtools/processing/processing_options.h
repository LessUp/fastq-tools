/**
 * @file processing_options.h
 * @brief 用户可见的处理选项与性能预设
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

namespace fq::processing {

/// 性能预设：调整 batch/缓冲区大小和在途 token 数
enum class ProcessingProfile : std::uint8_t {
    Default,        ///< 平衡模式
    LowMemory,      ///< 内存受限场景
    HighThroughput  ///< 最大吞吐量
};

/// 用户可见的处理选项，内部参数由预设自动生成
struct ProcessingOptions {
    size_t batchSize = 10000;                                ///< 批处理大小（记录数）
    size_t threadCount = 1;                                  ///< 线程数量（1 = 串行）
    ProcessingProfile profile = ProcessingProfile::Default;  ///< 性能预设
    std::optional<size_t> memoryLimitBytes;                  ///< 内存限制（字节），可选
    std::optional<size_t> batchCapacityBytes;  ///< 单批缓冲上限（字节），可选；覆盖预设默认值。
                                               ///< 超长记录（如 ONT read）需要调大此上限

    /// @throws std::invalid_argument 如果配置无效
    void validate() const;
};

}  // namespace fq::processing
