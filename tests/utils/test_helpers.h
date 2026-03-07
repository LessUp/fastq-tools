#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "fixture_loader.h"

namespace fq::test {

/**
 * @brief FASTQ 测试数据生成器
 *
 * 专注于生成测试用的 FASTQ 数据（记录、DNA 序列、质量分数）。
 * 文件操作请使用 FixtureLoader / TempDirectory。
 */
class TestDataGenerator {
public:
    /**
     * @brief 生成测试用的 FASTQ 记录
     * @param count 记录数量
     * @param readLength 读长
     * @return FASTQ 格式字符串
     */
    static auto generateFastQRecords(size_t count, size_t readLength = 100) -> std::string;

    /**
     * @brief 生成随机 DNA 序列
     * @param length 序列长度
     * @return DNA 序列字符串
     */
    static auto generateRandomDNA(size_t length) -> std::string;

    /**
     * @brief 生成随机质量分数（Phred+33 编码）
     * @param length 质量分数长度
     * @param minQuality 最小质量值
     * @param maxQuality 最大质量值
     * @return 质量分数字符串
     */
    static auto generateRandomQuality(size_t length,
                                      int minQuality = 20,
                                      int maxQuality = 40) -> std::string;

    /**
     * @brief 创建临时 FASTQ 文件
     * @param content FASTQ 内容
     * @param suffix 文件后缀
     * @return 临时文件路径（自动注册清理）
     */
    static auto createTempFile(const std::string& content,
                               const std::string& suffix = ".fastq") -> std::filesystem::path;

    /**
     * @brief 清理所有已注册的临时文件
     */
    static void cleanup();

private:
    static std::vector<std::filesystem::path> tempPaths_;
};

// 向后兼容别名
using TestHelpers = TestDataGenerator;

/**
 * @brief 测试基类，提供通用的 SetUp/TearDown
 *
 * 自动创建临时目录并在测试结束后清理。
 */
class FastQToolsTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    /// 每个测试用例独立的临时目录（RAII 自动清理）
    TempDirectory tempDir_;
    /// 测试数据目录
    std::filesystem::path testDataDir_;
};

/**
 * @brief 参数化测试基类
 */
template <typename T>
class FastQToolsParameterizedTest : public FastQToolsTest,
                                    public ::testing::WithParamInterface<T> {};

}  // namespace fq::test
