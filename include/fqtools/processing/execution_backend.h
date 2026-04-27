#pragma once

#include <cstdint>

namespace fq::processing {

/**
 * @brief 执行后端标识
 * @details 为后续 execution abstraction 预留公共配置表面。
 */
enum class ExecutionBackend : std::uint8_t {
    OneTbb,  ///< 默认后端：Intel oneTBB
};

}  // namespace fq::processing
