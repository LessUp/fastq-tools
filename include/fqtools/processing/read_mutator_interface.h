/**
 * @file read_mutator_interface.h
 * @brief 读段修改器接口定义
 * @details 定义用于修改 FASTQ 读段的修改器接口。
 *
 * 修改器（Mutator）用于对读段进行变换操作，
 * 例如：质量修剪、接头去除、长度截断等。
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
 * @brief 读段修改器接口
 * @details 抽象基类，用于定义读段修改操作。
 *
 * 实现自定义修改器：
 * @code
 * class MyMutator : public ReadMutatorInterface {
 * public:
 *     void process(fq::io::FastqRecord& read) override {
 *         // 修改 read 的内容
 *         // 注意：FastqRecord 是 string_view，需要通过 batch 修改
 *     }
 * };
 * @endcode
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
     *
     * @note 该方法会直接修改传入的读段对象
     */
    virtual void process(fq::io::FastqRecord& read) = 0;
};

}  // namespace fq::processing
