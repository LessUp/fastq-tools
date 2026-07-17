#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "types.h"

namespace fq::benchmark {

/**
 * @brief 结果存储管理器
 */
class ResultStorage {
public:
    explicit ResultStorage(
        const std::filesystem::path& baseDir = "docs/performance/benchmark-reports");

    /**
     * @brief 保存基准测试报告
     * @param report 报告数据
     * @return 保存的文件路径
     */
    std::filesystem::path saveResult(const BenchmarkReport& report);

    /**
     * @brief 加载指定的结果文件
     * @param filename 文件名
     * @return 报告数据
     */
    BenchmarkReport loadResult(const std::string& filename);

    /**
     * @brief 获取所有历史结果文件
     * @return 文件名列表（按时间排序）
     */
    [[nodiscard]] std::vector<std::string> listResults() const;

    /**
     * @brief 获取最新的结果
     * @return 最新的报告数据
     */
    BenchmarkReport getLatestResult();

    /**
     * @brief 保存为命名基线
     * @param report 报告数据
     * @param name 基线名称
     * @return 保存的文件路径
     */
    std::filesystem::path saveBaseline(const BenchmarkReport& report, const std::string& name);

    /**
     * @brief 加载命名基线
     * @param name 基线名称
     * @return 报告数据
     */
    BenchmarkReport loadBaseline(const std::string& name);

    /**
     * @brief 获取所有基线名称
     * @return 基线名称列表
     */
    [[nodiscard]] std::vector<std::string> listBaselines() const;

    /**
     * @brief 检查基线是否存在
     * @param name 基线名称
     * @return 是否存在
     */
    [[nodiscard]] bool baselineExists(const std::string& name) const;

private:
    std::filesystem::path baseDir_;
    std::filesystem::path resultsDir_;
    std::filesystem::path baselinesDir_;

    void ensureDirectories();
};

}  // namespace fq::benchmark
