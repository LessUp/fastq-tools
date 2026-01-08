#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace fq::test {

/**
 * @brief 测试数据加载器 - 统一管理测试数据文件
 */
class FixtureLoader {
public:
    /**
     * @brief 加载文本文件内容
     * @param path 文件路径
     * @return 文件内容
     */
    static std::string loadTextFile(const std::filesystem::path& path);

    /**
     * @brief 加载文件的所有行
     * @param path 文件路径
     * @return 行数组
     */
    static std::vector<std::string> loadLines(const std::filesystem::path& path);

    /**
     * @brief 获取测试数据文件路径
     * @param filename 文件名
     * @return 完整路径
     */
    static std::filesystem::path getFixturePath(const std::string& filename);

    /**
     * @brief 检查测试数据文件是否存在
     * @param filename 文件名
     * @return 是否存在
     */
    static bool fixtureExists(const std::string& filename);
    
    /**
     * @brief 创建临时 FASTQ 文件用于测试
     * @param records 记录数
     * @param read_length 读长
     * @return 临时文件路径
     */
    static std::filesystem::path createTempFastq(size_t records, size_t read_length = 100);
    
    /**
     * @brief 比较两个文件内容
     * @param file1 文件1路径
     * @param file2 文件2路径
     * @return 文件是否相同
     */
    static bool compareFiles(const std::filesystem::path& file1, 
                            const std::filesystem::path& file2);
};

/**
 * @brief 临时目录管理器 - RAII 风格自动清理
 */
class TempDirectory {
public:
    TempDirectory();
    explicit TempDirectory(const std::string& prefix);
    ~TempDirectory();
    
    // 禁止拷贝
    TempDirectory(const TempDirectory&) = delete;
    TempDirectory& operator=(const TempDirectory&) = delete;
    
    // 支持移动
    TempDirectory(TempDirectory&& other) noexcept;
    TempDirectory& operator=(TempDirectory&& other) noexcept;
    
    const std::filesystem::path& path() const { return path_; }
    operator std::filesystem::path() const { return path_; }
    
private:
    std::filesystem::path path_;
};

/**
 * @brief 性能计时器 - 用于测试性能
 */
class PerformanceTimer {
public:
    PerformanceTimer();
    
    void start();
    void stop();
    void reset();
    
    double elapsedSeconds() const;
    double elapsedMilliseconds() const;
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time_;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time_;
    bool running_;
};

}  // namespace fq::test
