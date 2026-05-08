#include "processing/execution_runtime.h"

#include <stdexcept>

namespace fq::processing {

ExecutionRuntime::ExecutionRuntime(std::unique_ptr<ExecutionRuntimeAdapter> adapter)
    : adapter_(std::move(adapter)) {
    if (!adapter_) {
        throw std::invalid_argument("ExecutionRuntime: adapter cannot be null");
    }
}

}  // namespace fq::processing
