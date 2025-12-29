#include "fqtools/processing/processing_pipeline_interface.h"
#include "fqtools/statistics/statistic_calculator_interface.h"

#include "processing/processing_pipeline.h"
#include "statistics/fq_statistic.h"

namespace fq::statistic {

/**
 * @brief factory function implementation.
 * This is the only place in the code (outside of the module itself)
 * that knows about the concrete FqStatistic class.
 */
auto createStatisticCalculator(const StatisticOptions& options)
    -> std::unique_ptr<StatisticCalculatorInterface> {
    return std::make_unique<FastqStatisticCalculator>(options);
}

}  // namespace fq::statistic

namespace fq::processing {

/**
 * @brief factory function for the processing pipeline.
 */
auto createProcessingPipeline() -> std::unique_ptr<ProcessingPipelineInterface> {
    return std::make_unique<SequentialProcessingPipeline>();
}

}  // namespace fq::processing
