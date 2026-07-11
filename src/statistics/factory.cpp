#include "fqtools/statistics/interfaces.h"

#include "statistics/fq_statistic.h"

namespace fq::statistic {

/**
 * @brief 统计计算器工厂函数实现
 * @details 创建 FastqStatisticCalculator 的具体实例，隐藏实现细节
 */
auto createStatisticCalculator(const StatisticOptions& options)
    -> std::unique_ptr<StatisticCalculatorInterface> {
    return std::make_unique<FastqStatisticCalculator>(options);
}

}  // namespace fq::statistic
