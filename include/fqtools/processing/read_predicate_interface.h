/**
 * @file read_predicate_interface.h
 * @brief 读段谓词接口定义
 * @details 定义用于过滤 FASTQ 读段的谓词接口。
 *
 * 谓词（Predicate）用于判断一条读段是否满足特定条件，
 * 例如：最小质量值、最小长度、GC 含量等。
 *
 * @author LessUp
 * @date 2023-10-05
 * @version 1.0
 * @copyright (c) 2023 LessUp. All rights reserved.
 */

#pragma once

#include "fqtools/io/fastq_io.h"

namespace fq::processing {

/**
 * @brief 读段谓词接口
 * @details 抽象基类，用于定义读段过滤条件。
 *
 * 实现自定义谓词：
 * @code
 * class MyPredicate : public ReadPredicateInterface {
 * public:
 *     auto evaluate(const fq::io::FastqRecord& read) const -> bool override {
 *         // 自定义过滤逻辑
 *         return read.length() >= minLength_;
 *     }
 * private:
 *     size_t minLength_;
 * };
 * @endcode
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
    virtual auto evaluate(const fq::io::FastqRecord& read) const -> bool = 0;
};

}  // namespace fq::processing
