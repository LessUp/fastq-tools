/**
 * @file interfaces.h
 * @brief 处理管道算子接口：ReadPredicateInterface（过滤）与 ReadMutatorInterface（修改）
 */

#pragma once

#include "fqtools/io/fastq_io.h"

namespace fq::processing {

/**
 * @brief 读段谓词接口
 * @details 判断读段是否满足过滤条件（true = 保留，false = 过滤）
 */
class ReadPredicateInterface {
public:
    virtual ~ReadPredicateInterface() = default;

    [[nodiscard]] virtual auto evaluate(const fq::io::FastqRecord& read) const -> bool = 0;
};

/**
 * @brief 读段修改器接口
 * @details 对读段进行变换（修剪、截断等）。修改 FastqRecord 时需注意其指向 Batch 内存。
 */
class ReadMutatorInterface {
public:
    virtual ~ReadMutatorInterface() = default;

    virtual void process(fq::io::FastqRecord& read) = 0;
};

}  // namespace fq::processing
