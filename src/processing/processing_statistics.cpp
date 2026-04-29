#include "fqtools/processing/processing_pipeline_interface.h"

#include <iomanip>
#include <sstream>

namespace fq::processing {

namespace {

auto memoryPolicyName(MemoryResourcePolicy policy) -> const char* {
    switch (policy) {
        case MemoryResourcePolicy::ObjectPool:
            return "objectPool";
    }

    return "unknown";
}

}  // namespace

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
    if (allocationTelemetryEnabled) {
        oss << "\n  内存策略: " << memoryPolicyName(memoryResourcePolicy);
        oss << "\n  In-flight 批次数上限: " << resolvedMaxInFlightBatches;
    }

    return oss.str();
}

}  // namespace fq::processing
