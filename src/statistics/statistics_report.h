#pragma once

#include "fqtools/statistics/statistics_writer.h"

#include <string>
#include <vector>

namespace fq::statistic {

struct StatisticsReport {
    std::vector<std::string> summaryLines;
    std::vector<std::string> positionLines;
    std::vector<std::string> signatureLines;
};

[[nodiscard]] auto buildStatisticsReport(
    const FqStatisticResult& result, const StatisticsWriterOptions& options) -> StatisticsReport;

}  // namespace fq::statistic
