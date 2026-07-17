/**
 * @file processing_pipeline_interface.h
 * @brief 处理管道接口定义
 * @details 对外只暴露一个 move-only 的具体 Pipeline；实现细节通过 PIMPL 隐藏。
 *
 * @author FastQTools Team
 * @date 2026
 * @version 2.0
 *
 * @copyright Copyright (c) 2026 FastQTools
 * @license MIT License
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
    uint64_t totalReads = 0;        ///< 总读取数
    uint64_t passedReads = 0;       ///< 通过筛选的读取数
    uint64_t filteredReads = 0;     ///< 被过滤的读取数
    uint64_t modifiedReads = 0;     ///< 被修改的读取数
    uint64_t errorReads = 0;        ///< 出错的读取数
    uint64_t inputBytes = 0;        ///< 输入字节数（解压后的原始文本字节）
    uint64_t outputBytes = 0;       ///< 输出字节数（写出前的原始文本字节）
    uint64_t elapsedMs = 0;         ///< 处理时间（毫秒）
    double processingTimeMs = 0.0;  ///< 处理时间（毫秒，浮点数，保留兼容）
    double throughputMbps = 0.0;    ///< 吞吐量（MB/s）

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

    /**
     * @brief 设置输入文件路径
     * @details 指定要处理的 FastQ 文件路径
     *
     * @param inputPath 输入文件路径
     * @pre inputPath 必须是有效的文件路径
     * @post 输入路径被设置
     * @note 如果已通过 setReader 设置了自定义 Reader，此路径可能被忽略
     */
    void setInputPath(const std::string& inputPath);

    /**
     * @brief 设置输出文件路径
     * @details 指定处理结果的输出文件路径
     *
     * @param outputPath 输出文件路径
     * @pre outputPath 必须是有效的文件路径
     * @post 输出路径被设置
     * @note 如果已通过 setWriter 设置了自定义 Writer，此路径可能被忽略
     */
    void setOutputPath(const std::string& outputPath);

    /**
     * @brief 设置自定义 Reader
     * @details 注入自定义的 FASTQ 读取器，用于测试或特殊数据源
     *
     * @param reader FASTQ 读取器实例
     * @post 使用注入的 reader 替代默认的文件读取器
     * @note 测试时可用于注入 mock reader
     */
    void setReader(std::unique_ptr<fq::io::IReader> reader);

    /**
     * @brief 设置自定义 Writer
     * @details 注入自定义的 FASTQ 写入器，用于测试或特殊输出目标
     *
     * @param writer FASTQ 写入器实例
     * @post 使用注入的 writer 替代默认的文件写入器
     * @note 测试时可用于注入 mock writer
     */
    void setWriter(std::unique_ptr<fq::io::IWriter> writer);

    /**
     * @brief 设置处理选项
     * @details 配置处理参数，包括批处理大小、线程数和性能预设
     *
     * @param options 处理选项结构体
     * @pre options 必须包含有效的配置参数
     * @post 配置被应用到处理管道
     * @throw std::invalid_argument 如果配置参数无效
     */
    void setProcessingOptions(const ProcessingOptions& options);

    /**
     * @brief 添加数据修改器
     * @details 注册一个数据修改器，用于对读取数据进行转换处理
     *
     * @param mutator 数据修改器的唯一指针
     * @pre mutator 必须是有效的修改器实例
     * @post 修改器被注册到处理管道中
     * @note 修改器按添加顺序依次执行
     */
    void addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator);

    /**
     * @brief 添加数据过滤器
     * @details 注册一个数据过滤器，用于筛选符合条件的读取
     *
     * @param predicate 数据过滤器的唯一指针
     * @pre predicate 必须是有效的过滤器实例
     * @post 过滤器被注册到处理管道中
     * @note 过滤器按添加顺序依次执行，任一过滤器失败则数据被过滤
     */
    void addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate);

    /**
     * @brief 执行数据处理
     * @details 启动完整的 FastQ 数据处理流程
     *
     * @return ProcessingStatistics 处理统计信息
     * @pre 输入路径、输出路径和配置必须已设置
     * @post 处理结果写入输出文件
     * @throw std::runtime_error 如果处理过程中发生错误
     */
    [[nodiscard]] auto run() -> ProcessingStatistics;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fq::processing
