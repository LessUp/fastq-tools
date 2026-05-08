/**
 * @file processing_options.cpp
 * @brief ProcessingOptions 验证实现
 *
 * @author LessUp
 * @date 2026-05-08
 * @version 2.0
 * @copyright (c) 2026 LessUp. All rights reserved.
 */

#include "fqtools/processing/processing_options.h"

#include <stdexcept>

namespace fq::processing {

void ProcessingOptions::validate() const {
    if (batchSize == 0) {
        throw std::invalid_argument("batchSize must be greater than 0");
    }
    if (threadCount == 0) {
        throw std::invalid_argument("threadCount must be greater than 0");
    }
    if (memoryLimitBytes.has_value() && memoryLimitBytes.value() == 0) {
        throw std::invalid_argument("memoryLimitBytes must be greater than 0 if specified");
    }
}

}  // namespace fq::processing
