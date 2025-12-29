#include "fqtools/processing/processing_pipeline_interface.h"

#include <iomanip>
#include <sstream>

namespace fq::processing {

auto ProcessingStatistics::toString() const -> std::string {
    std::ostringstream oss;

    oss << "处理统计信息:\n";
    oss << "  总读取数: " << totalReads << "\n";
    oss << "  通过读取数: " << passedReads << " (" << std::fixed << std::setprecision(2)
        << getPassRate() * 100.0 << "%)\n";
    oss << "  过滤读取数: " << filteredReads << " (" << std::fixed << std::setprecision(2)
        << getFilterRate() * 100.0 << "%)\n";
    oss << "  修改读取数: " << modifiedReads << "\n";
    oss << "  错误读取数: " << errorReads << "\n";
    oss << "  处理时间: " << std::fixed << std::setprecision(2) << processingTimeMs << " ms\n";
    oss << "  处理吞吐量: " << std::fixed << std::setprecision(2) << throughputMbps << " MB/s";

    return oss.str();
}

}  // namespace fq::processing
