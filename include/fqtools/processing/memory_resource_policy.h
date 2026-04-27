#pragma once

#include <cstdint>

namespace fq::processing {

/**
 * @brief 内存资源策略标识
 * @details 为后续 memory-resource-policy 演进预留公共配置表面。
 */
enum class MemoryResourcePolicy : std::uint8_t {
    ObjectPool,  ///< 默认策略：沿用当前对象池复用路径
};

}  // namespace fq::processing
