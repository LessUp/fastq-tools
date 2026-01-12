#pragma once

#include <nlohmann/json.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace fq::benchmark {

/**
 * @brief 基准测试结果
 */
struct BenchmarkResult {
    std::string name;                    ///< 测试名称
    std::string category;                ///< 类别: "io", "filter", "stat"
    std::size_t iterations{0};           ///< 迭代次数

    double meanTimeNs{0.0};              ///< 平均时间 (纳秒)
    double stdDevNs{0.0};                ///< 标准差 (纳秒)
    double minTimeNs{0.0};               ///< 最小时间 (纳秒)
    double maxTimeNs{0.0};               ///< 最大时间 (纳秒)
    double throughputMBps{0.0};          ///< 吞吐量 (MB/s)
    double throughputReadsPerSec{0.0};   ///< 吞吐量 (reads/s)
    std::size_t peakMemoryBytes{0};      ///< 峰值内存使用 (bytes)
    std::size_t threadCount{1};          ///< 线程数
    std::size_t inputSize{0};            ///< 输入大小 (reads 数量)
};

/**
 * @brief 系统元数据
 */
struct SystemMetadata {
    std::string timestamp;               ///< ISO 8601 时间戳
    std::string gitCommit;               ///< Git commit hash
    std::string gitBranch;               ///< Git 分支名
    std::string cpuModel;                ///< CPU 型号
    int coreCount{0};                    ///< CPU 核心数
    std::size_t memoryBytes{0};          ///< 系统内存 (bytes)
    std::string osVersion;               ///< 操作系统版本
    std::string compilerVersion;         ///< 编译器版本
};

/**
 * @brief 完整的基准测试报告
 */
struct BenchmarkReport {
    SystemMetadata metadata;             ///< 系统元数据
    std::vector<BenchmarkResult> results; ///< 测试结果列表
};

// JSON 序列化支持
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    BenchmarkResult,
    name, category, iterations,
    meanTimeNs, stdDevNs, minTimeNs, maxTimeNs,
    throughputMBps, throughputReadsPerSec,
    peakMemoryBytes, threadCount, inputSize
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    SystemMetadata,
    timestamp, gitCommit, gitBranch,
    cpuModel, coreCount, memoryBytes,
    osVersion, compilerVersion
)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    BenchmarkReport,
    metadata, results
)

}  // namespace fq::benchmark
