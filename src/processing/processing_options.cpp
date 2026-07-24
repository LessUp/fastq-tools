/**
 * @file processing_options.cpp
 * @brief ProcessingOptions 验证实现
 *
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
