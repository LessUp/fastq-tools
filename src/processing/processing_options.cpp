/**
 * @file processing_options.cpp
 * @brief ProcessingOptions 验证实现
 *
 */

#include "fqtools/processing/processing_options.h"

#include <stdexcept>
#include <string>

namespace fq::processing {

void ProcessingOptions::validate() const {
    if (batchSize == 0) {
        throw std::invalid_argument("batchSize must be greater than 0");
    }
    // 批次按记录数预分配记录视图数组，超大值会在运行期直接 bad_alloc/OOM，
    // 必须在参数校验阶段拒绝（100 万条/批已是极端配置）
    constexpr size_t kMaxBatchSize = 1'000'000;
    if (batchSize > kMaxBatchSize) {
        throw std::invalid_argument("batchSize must be <= " + std::to_string(kMaxBatchSize));
    }
    if (threadCount == 0) {
        throw std::invalid_argument("threadCount must be greater than 0");
    }
    if (memoryLimitBytes.has_value() && memoryLimitBytes.value() == 0) {
        throw std::invalid_argument("memoryLimitBytes must be greater than 0 if specified");
    }
    if (batchCapacityBytes.has_value() && batchCapacityBytes.value() == 0) {
        throw std::invalid_argument("batchCapacityBytes must be greater than 0 if specified");
    }
}

}  // namespace fq::processing
