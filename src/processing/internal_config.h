/**
 * @file internal_config.h
 * @brief 内部配置内部头文件
 * @details 供实现层使用的 InternalConfig 声明和辅助函数
 *
 * @author LessUp
 * @date 2026-05-08
 * @version 2.0
 * @copyright (c) 2026 LessUp. All rights reserved.
 */

#pragma once

#include "fqtools/processing/processing_options.h"

#include <cstddef>

namespace fq::processing {

/**
 * @brief 根据内存限制计算最大并行批次数
 * @param memoryLimitBytes 内存限制（字节）
 * @param batchCapacityBytes 批次容量（字节）
 * @param threadCount 线程数
 * @return 最大并行批次数（0 表示自动）
 */
[[nodiscard]] auto calculateMaxInFlightBatches(size_t memoryLimitBytes,
                                               size_t batchCapacityBytes,
                                               size_t threadCount) -> size_t;

}  // namespace fq::processing
