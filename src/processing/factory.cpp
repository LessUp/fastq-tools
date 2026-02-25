#include "fqtools/processing/processing_pipeline_interface.h"

#include "processing/processing_pipeline.h"

namespace fq::processing {

/**
 * @brief 处理管道工厂函数实现
 * @details 创建 ProcessingPipeline 的具体实例，隐藏实现细节
 */
auto createProcessingPipeline() -> std::unique_ptr<ProcessingPipelineInterface> {
    return std::make_unique<ProcessingPipeline>();
}

}  // namespace fq::processing
