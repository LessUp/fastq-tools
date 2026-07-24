/**
 * @file processing_pipeline_interface.h
 * @brief 处理管道接口定义
 * @details 对外只暴露一个 move-only 的具体 Pipeline；实现细节通过 PIMPL 隐藏。
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "fqtools/io/interfaces.h"
#include "fqtools/processing/interfaces.h"
#include "fqtools/processing/processing_options.h"

#include <cstdint>
#include <memory>
#include <string>

namespace fq::processing {

/**
 * @brief 处理统计信息结构体
 * @details 记录 FastQ 数据处理过程中的各项统计指标，用于性能监控和结果分析
 */
struct ProcessingStatistics {
    uint64_t totalReads = 0;      ///< 总读取数
    uint64_t passedReads = 0;     ///< 通过筛选的读取数
    uint64_t filteredReads = 0;   ///< 被过滤的读取数
    uint64_t modifiedReads = 0;   ///< 被修改的读取数
    uint64_t errorReads = 0;      ///< 出错的读取数
    uint64_t inputBytes = 0;      ///< 输入字节数（解压后的原始文本字节）
    uint64_t outputBytes = 0;     ///< 输出字节数（写出前的原始文本字节）
    uint64_t elapsedMs = 0;       ///< 处理时间（毫秒）
    double throughputMbps = 0.0;  ///< 吞吐量（MB/s）

    /**
     * @brief 获取通过率
     * @return 通过率（0.0 ~ 1.0）
     */
    [[nodiscard]] auto getPassRate() const -> double {
        return totalReads > 0 ? static_cast<double>(passedReads) / static_cast<double>(totalReads)
                              : 0.0;
    }

    /**
     * @brief 获取过滤率
     * @return 过滤率（0.0 ~ 1.0）
     */
    [[nodiscard]] auto getFilterRate() const -> double {
        return totalReads > 0 ? static_cast<double>(filteredReads) / static_cast<double>(totalReads)
                              : 0.0;
    }

    /**
     * @brief 转换为字符串表示
     * @return 统计信息的可读字符串
     */
    [[nodiscard]] auto toString() const -> std::string;
};

/**
 * @brief FastQ 处理管道
 * @details move-only 的具体 API，内部通过 PIMPL 隐藏执行后端和状态。
 */
class Pipeline {
public:
    Pipeline();
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    auto operator=(const Pipeline&) -> Pipeline& = delete;
    Pipeline(Pipeline&&) noexcept;
    auto operator=(Pipeline&&) noexcept -> Pipeline&;

    /// @brief 设置输入文件路径
    /// @note 如果已通过 setReader 设置了自定义 Reader，此路径可能被忽略
    void setInputPath(const std::string& inputPath);

    /// @brief 设置输出文件路径
    /// @note 如果已通过 setWriter 设置了自定义 Writer，此路径可能被忽略
    void setOutputPath(const std::string& outputPath);

    /// @brief 注入自定义 Reader（测试或特殊数据源）
    void setReader(std::unique_ptr<fq::io::IReader> reader);

    /// @brief 注入自定义 Writer（测试或特殊输出目标）
    void setWriter(std::unique_ptr<fq::io::IWriter> writer);

    /// @brief 设置处理选项（批处理大小、线程数、性能预设）
    void setProcessingOptions(const ProcessingOptions& options);

    /// @brief 添加修改器，按添加顺序依次执行
    void addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator);

    /// @brief 添加过滤器，按添加顺序依次执行，任一失败则过滤
    void addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate);

    /// @brief 执行处理，返回统计信息
    [[nodiscard]] auto run() -> ProcessingStatistics;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fq::processing
