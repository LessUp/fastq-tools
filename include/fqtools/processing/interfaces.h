/**
 * @file interfaces.h
 * @brief FASTQ 处理管道接口集合
 * @details 定义读段谓词和修改器接口，用于过滤和变换 FASTQ 读段。
 */

#pragma once

#include "fqtools/io/fastq_io.h"

namespace fq::processing {

/**
 * @brief 读段谓词接口
 * @details 抽象基类，用于定义读段过滤条件。
 *
 * 谓词（Predicate）用于判断一条读段是否满足特定条件，
 * 例如：最小质量值、最小长度、GC 含量等。
 *
 * @note 所有谓词实现必须重写 evaluate() 方法
 */
class ReadPredicateInterface {
public:
    virtual ~ReadPredicateInterface() = default;

    /**
     * @brief 评估读段是否满足条件
     * @param read 待评估的 FASTQ 读段
     * @return 是否满足过滤条件（true = 保留，false = 过滤掉）
     */
    [[nodiscard]] virtual auto evaluate(const fq::io::FastqRecord& read) const -> bool = 0;
};

/**
 * @brief 读段修改器接口
 * @details 抽象基类，用于定义读段修改操作。
 *
 * 修改器（Mutator）用于对读段进行变换操作，
 * 例如：质量修剪、接头去除、长度截断等。
 *
 * @note 所有修改器实现必须重写 process() 方法
 * @warning 修改 FastqRecord 时需注意其指向 Batch 内存
 */
class ReadMutatorInterface {
public:
    virtual ~ReadMutatorInterface() = default;

    /**
     * @brief 处理读段
     * @param read 待处理的 FASTQ 读段（可能被修改）
     */
    virtual void process(fq::io::FastqRecord& read) = 0;
};

}  // namespace fq::processing
