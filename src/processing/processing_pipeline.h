#pragma once

#include "fqtools/processing/processing_pipeline_interface.h"

#include <memory>
#include <vector>

#include "processing/execution_runtime.h"

namespace fq::processing {

class ReadMutatorInterface;
class ReadPredicateInterface;

/**
 * @brief FastQ 数据处理管道实现类
 * @details 该类实现了 FastQ 文件的完整处理流程，包括：
 *          - 文件读取和解析
 *          - 数据过滤和转换
 *          - 并行处理支持
 *          - 结果写入和统计
 *
 * 支持串行和并行（TBB）两种处理模式，可根据配置自动选择最优策略
 *
 * @invariant 配置设置必须在运行前完成
 * @warning 处理大文件时建议使用并行模式以提高性能
 */
class ProcessingPipeline : public ProcessingPipelineInterface {
public:
    /**
     * @brief 默认构造函数
     * @details 初始化处理管道，设置默认配置
     * @post 管道处于就绪状态，可以接受配置
     */
    ProcessingPipeline();

    /**
     * @brief 析构函数
     * @details 清理所有资源，确保文件句柄正确关闭
     */
    ~ProcessingPipeline();

    void setInputPath(const std::string& inputPath) override;
    void setOutputPath(const std::string& outputPath) override;
    void setReader(std::unique_ptr<fq::io::IReader> reader) override;
    void setWriter(std::unique_ptr<fq::io::IWriter> writer) override;
    void setProcessingOptions(const ProcessingOptions& options) override;
    void addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) override;
    void addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) override;
    auto run() -> ProcessingStatistics override;

private:
    /**
     * @brief 处理数据批次
     * @details 对一批 FastQ 数据进行处理，应用所有修改器和过滤器
     */
    auto processBatch(fq::io::FastqBatch& batch, ProcessingStatistics& stats) -> bool;

    ExecutionRuntime runtime_;
    std::string inputPath_;                                        ///< 输入文件路径
    std::string outputPath_;                                       ///< 输出文件路径
    ProcessingOptions options_;                                    ///< 用户可见的处理选项
    std::vector<std::unique_ptr<ReadMutatorInterface>> mutators_;  ///< 数据修改器列表
    std::vector<std::unique_ptr<ReadPredicateInterface>> predicates_;  ///< 数据过滤器列表
    std::unique_ptr<fq::io::IReader> customReader_;  ///< 自定义读取器（测试用）
    std::shared_ptr<fq::io::IWriter> customWriter_;  ///< 自定义写入器（测试用）
};

}  // namespace fq::processing
