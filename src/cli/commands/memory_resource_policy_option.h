#pragma once

#include "fqtools/processing/memory_resource_policy.h"

#include <stdexcept>
#include <string>
#include <string_view>

namespace fq::cli::commands {

inline auto parseMemoryResourcePolicy(std::string_view value) -> fq::processing::MemoryResourcePolicy {
    if (value == "objectPool") {
        return fq::processing::MemoryResourcePolicy::ObjectPool;
    }

    throw std::invalid_argument("Unsupported memory resource policy: " + std::string(value));
}

}  // namespace fq::cli::commands
