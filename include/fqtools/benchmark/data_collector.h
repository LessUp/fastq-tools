#pragma once

#include <fqtools/benchmark/types.h>

#include <string>

namespace fq::benchmark {

/**
 * @brief 数据收集器 - 收集系统信息和序列化结果
 */
class DataCollector {
public:
    /**
     * @brief 收集系统元数据
     * @return 填充的 SystemMetadata 结构
     */
    static SystemMetadata collectSystemMetadata();

    /**
     * @brief 获取当前 Git commit hash
     * @return commit hash 字符串，失败返回 "unknown"
     */
    static std::string getGitCommit();

    /**
     * @brief 获取当前 Git 分支名
     * @return 分支名字符串，失败返回 "unknown"
     */
    static std::string getGitBranch();

    /**
     * @brief 获取 CPU 型号
     * @return CPU 型号字符串
     */
    static std::string getCpuModel();

    /**
     * @brief 获取 CPU 核心数
     * @return 核心数
     */
    static int getCoreCount();

    /**
     * @brief 获取系统内存大小
     * @return 内存大小 (bytes)
     */
    static std::size_t getMemoryBytes();

    /**
     * @brief 获取操作系统版本
     * @return OS 版本字符串
     */
    static std::string getOsVersion();

    /**
     * @brief 获取编译器版本
     * @return 编译器版本字符串
     */
    static std::string getCompilerVersion();

    /**
     * @brief 获取当前 ISO 8601 时间戳
     * @return 时间戳字符串
     */
    static std::string getTimestamp();

    /**
     * @brief 将报告序列化为 JSON 字符串
     * @param report 基准测试报告
     * @return JSON 字符串
     */
    static std::string toJson(const BenchmarkReport& report);

    /**
     * @brief 从 JSON 字符串反序列化报告
     * @param json JSON 字符串
     * @return 基准测试报告
     */
    static BenchmarkReport fromJson(const std::string& json);

    /**
     * @brief 生成结果文件名
     * @param metadata 系统元数据
     * @return 格式: YYYY-MM-DD_HH-MM-SS_<commit>.json
     */
    static std::string generateResultFilename(const SystemMetadata& metadata);
};

}  // namespace fq::benchmark
