#include "fqtools/processing/processing_pipeline_interface.h"

#include <fmt/format.h>

namespace fq::processing {

auto ProcessingStatistics::toString() const -> std::string {
    return fmt::format(
        "处理统计信息:\n"
        "  总读取数: {}\n"
        "  通过读取数: {} ({:.2f}%)\n"
        "  过滤读取数: {} ({:.2f}%)\n"
        "  修改读取数: {}\n"
        "  处理时间: {} ms\n"
        "  处理吞吐量: {:.2f} MB/s",
        totalReads,
        passedReads,
        getPassRate() * 100.0,
        filteredReads,
        getFilterRate() * 100.0,
        modifiedReads,
        elapsedMs,
        throughputMbps);
}

}  // namespace fq::processing
