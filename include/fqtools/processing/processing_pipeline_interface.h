/**
 * @file processing_pipeline_interface.h
 * @brief 处理管道公共 API：move-only Pipeline 与 ProcessingStatistics
 */

#pragma once

#include "fqtools/io/interfaces.h"
#include "fqtools/processing/interfaces.h"
#include "fqtools/processing/processing_options.h"

#include <cstdint>
#include <memory>
#include <string>

namespace fq::processing {

/// 处理统计信息
struct ProcessingStatistics {
    uint64_t totalReads = 0;      ///< 总读取数
    uint64_t passedReads = 0;     ///< 通过筛选的读取数
    uint64_t filteredReads = 0;   ///< 被过滤的读取数
    uint64_t modifiedReads = 0;   ///< 被修改的读取数
    uint64_t inputBytes = 0;      ///< 输入字节数（解压后的原始文本字节）
    uint64_t outputBytes = 0;     ///< 输出字节数（写出前的原始文本字节）
    uint64_t elapsedMs = 0;       ///< 处理时间（毫秒）
    double throughputMbps = 0.0;  ///< 吞吐量（MB/s）

    [[nodiscard]] auto getPassRate() const -> double {
        return totalReads > 0 ? static_cast<double>(passedReads) / static_cast<double>(totalReads)
                              : 0.0;
    }

    [[nodiscard]] auto getFilterRate() const -> double {
        return totalReads > 0 ? static_cast<double>(filteredReads) / static_cast<double>(totalReads)
                              : 0.0;
    }

    [[nodiscard]] auto toString() const -> std::string;
};

/**
 * @brief FastQ 处理管道（move-only）
 * @details 通过 PIMPL 隐藏执行后端。支持注入自定义 I/O 和 Predicate/Mutator。
 */
class Pipeline {
public:
    Pipeline();
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    auto operator=(const Pipeline&) -> Pipeline& = delete;
    Pipeline(Pipeline&&) noexcept;
    auto operator=(Pipeline&&) noexcept -> Pipeline&;

    /// 设置输入文件路径（自定义 Reader 优先）
    void setInputPath(const std::string& inputPath);

    /// 设置输出文件路径（自定义 Writer 优先）
    void setOutputPath(const std::string& outputPath);

    /// 注入自定义 reader（单次消费）：一次 run() 后必须重新注入才能再次运行
    void setReader(std::unique_ptr<fq::io::IReader> reader);
    /// 注入自定义 writer（共享所有权，调用方可控制 flush 生命周期）：
    /// run() 成功返回时 finish() 已被调用，重跑需注入新 writer
    void setWriter(std::unique_ptr<fq::io::IWriter> writer);
    void setProcessingOptions(const ProcessingOptions& options);

    /// 添加修改器，按添加顺序依次执行
    void addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator);

    /// 添加过滤器，按添加顺序依次执行，任一失败则过滤
    void addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate);

    [[nodiscard]] auto run() -> ProcessingStatistics;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace fq::processing
