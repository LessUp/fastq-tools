#pragma once

#include "fqtools/processing/processing_pipeline_interface.h"

#include <memory>
#include <vector>

namespace fq::processing {

class ReadMutatorInterface;
class ReadPredicateInterface;

/// @brief FastQ 数据处理管道实现类
/// @details 支持串行和并行（TBB）两种处理模式，根据配置自动选择。
class ProcessingPipeline {
public:
    ProcessingPipeline();
    ~ProcessingPipeline();

    void setInputPath(const std::string& inputPath);
    void setOutputPath(const std::string& outputPath);
    void setReader(std::unique_ptr<fq::io::IReader> reader);
    void setWriter(std::unique_ptr<fq::io::IWriter> writer);
    void setProcessingOptions(const ProcessingOptions& options);
    void addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator);
    void addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate);
    auto run() -> ProcessingStatistics;

private:
    /// @brief 对一批数据应用所有修改器和过滤器
    auto processBatch(fq::io::FastqBatch& batch, ProcessingStatistics& stats) -> void;

    std::string inputPath_;                                            ///< 输入文件路径
    std::string outputPath_;                                           ///< 输出文件路径
    ProcessingOptions options_;                                        ///< 用户可见的处理选项
    std::vector<std::unique_ptr<ReadMutatorInterface>> mutators_;      ///< 数据修改器列表
    std::vector<std::unique_ptr<ReadPredicateInterface>> predicates_;  ///< 数据过滤器列表
    std::unique_ptr<fq::io::IReader> customReader_;                    ///< 自定义读取器（测试用）
    std::shared_ptr<fq::io::IWriter> customWriter_;                    ///< 自定义写入器（测试用）
    bool customReaderConfigured_ = false;
};

}  // namespace fq::processing
