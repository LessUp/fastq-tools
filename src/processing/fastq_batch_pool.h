// Runtime-internal FastqBatch pool. It is intentionally not part of the installed API.
#pragma once

#include "fqtools/io/fastq_io.h"

#include <memory>

#include "processing/object_pool.h"

namespace fq::processing {

using FastqBatchPool = fq::memory::ObjectPool<fq::io::FastqBatch>;

inline void resetFastqBatch(fq::io::FastqBatch& batch) {
    batch.clear();
}

inline auto createFastqBatchPool(size_t initialSize,
                                 size_t maxSize,
                                 size_t batchCapacityBytes = 4 * 1024 * 1024,
                                 size_t batchSize = 10000) -> std::shared_ptr<FastqBatchPool> {
    return std::make_shared<FastqBatchPool>(
        initialSize, maxSize, resetFastqBatch, [batchCapacityBytes, batchSize] {
            return std::make_unique<fq::io::FastqBatch>(batchCapacityBytes, batchSize);
        });
}

}  // namespace fq::processing
