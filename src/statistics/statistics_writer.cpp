/**
 * @file statistics_writer.cpp
 * @brief 统计结果输出器实现
 */

#include "fqtools/statistics/statistics_writer.h"

#include "fqtools/logging.h"

#include <utility>

#include "statistics/statistics_report.h"

namespace fq::statistics {

StatisticsWriter::StatisticsWriter(StatisticsWriterOptions options)
    : options_(std::move(options)) {}

void StatisticsWriter::write(std::ostream& os, const FqStatisticResult& result) {
    const auto report = buildStatisticsReport(result, options_);
    if (report.summaryLines.empty()) {
        fq::logging::warn("No reads found in input file.");
        return;
    }

    for (const auto& line : report.summaryLines) {
        os << line << "\n";
    }
    for (const auto& line : report.positionLines) {
        os << line << "\n";
    }
}

void StatisticsWriter::writeSignature(std::ostream& os, const FqStatisticResult& result) {
    const auto report = buildStatisticsReport(result, options_);
    for (const auto& line : report.signatureLines) {
        os << line << "\n";
    }
}

void StatisticsWriter::writeJson(std::ostream& os, const FqStatisticResult& result) {
    os << formatStatisticsJson(result, options_);
}

}  // namespace fq::statistics
