#pragma once

#include "fqtools/processing/execution_backend.h"

#include <stdexcept>
#include <string>
#include <string_view>

namespace fq::cli::commands {

inline auto parseExecutionBackend(std::string_view value) -> fq::processing::ExecutionBackend {
    if (value == "oneTbb") {
        return fq::processing::ExecutionBackend::OneTbb;
    }

    throw std::invalid_argument("Unsupported execution backend: " + std::string(value));
}

}  // namespace fq::cli::commands
