/**
 * @file fastq_batch_pool.h
 * @brief FastqBatch 对象池类型定义
 * @details 提供 FastqBatch 专用的对象池类型别名和工厂函数
 *
 * @author FastQTools Team
 * @date 2024
 * @version 1.0
 *
 * @copyright Copyright (c) 2024 FastQTools
 * @license MIT License
 */

#pragma once

#include "fqtools/io/fastq_io.h"
#include "fqtools/memory/object_pool.h"

#include <memory>

namespace fq::io {

/**
 * @brief FastqBatch 默认重置函数
 * @param batch 要重置的批次对象
 * @details 清空批次中的所有记录和缓冲区数据，但保留已分配的内存容量
 */
inline void resetFastqBatch(FastqBatch& batch) {
    batch.clear();
}

/// FastqBatch 对象池类型别名
using FastqBatchPool = fq::memory::ObjectPool<FastqBatch>;

/**
 * @brief 创建 FastqBatch 对象池
 * @param initialSize 初始预分配的批次数量
 * @param maxSize 池的最大容量，0 表示无限制
 * @return 指向对象池的 shared_ptr
 *
 * @details 创建一个预配置的 FastqBatch 对象池，使用默认的重置函数。
 *          返回 shared_ptr 以支持 acquire() 方法中的 weak_ptr 机制。
 *
 * @example
 * @code
 * auto pool = createFastqBatchPool(4, 16);
 * auto batch = pool->acquire();
 * // 使用 batch...
 * @endcode
 */
inline auto createFastqBatchPool(size_t initialSize, size_t maxSize)
    -> std::shared_ptr<FastqBatchPool> {
    return std::make_shared<FastqBatchPool>(initialSize, maxSize, resetFastqBatch);
}

}  // namespace fq::io
