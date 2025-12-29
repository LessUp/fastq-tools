#include <filesystem>
#include <fstream>

#include "fqtools/config/config.h"
#include "fqtools/error/error.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace fq::config {

class ConfigurationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前清理全局配置
        globalConfig().clear();
    }

    void TearDown() override {
        // 清理测试文件
        if (std::filesystem::exists("test_config.txt")) {
            std::filesystem::remove("test_config.txt");
        }
    }

    // 创建测试配置文件
    void createTestConfigFile(const std::string& content) {
        std::ofstream file("test_config.txt");
        file << content;
        file.close();
    }
};

TEST_F(ConfigurationTest, EmptyConfiguration) {
    Configuration config;
    EXPECT_TRUE(config.empty());
    EXPECT_EQ(config.size(), 0);
}

TEST_F(ConfigurationTest, SetAndGetBool) {
    Configuration config;

    config.set("debugMode", true);
    config.set("verbose", false);

    EXPECT_TRUE(config.get<bool>("debugMode"));
    EXPECT_FALSE(config.get<bool>("verbose"));
}

TEST_F(ConfigurationTest, SetAndGetInt) {
    Configuration config;

    config.set("threadCount", int64_t(8));
    config.set("batchSize", int64_t(1000));

    EXPECT_EQ(config.get<int64_t>("threadCount"), 8);
    EXPECT_EQ(config.get<int64_t>("batchSize"), 1000);
}

TEST_F(ConfigurationTest, SetAndGetDouble) {
    Configuration config;

    config.set("qualityThreshold", 20.5);
    config.set("memoryLimit", 1024.0);

    EXPECT_DOUBLE_EQ(config.get<double>("qualityThreshold"), 20.5);
    EXPECT_DOUBLE_EQ(config.get<double>("memoryLimit"), 1024.0);
}

TEST_F(ConfigurationTest, SetAndGetString) {
    Configuration config;

    config.set("inputFile", "test.fastq");
    config.set("outputDir", "/tmp/output");

    EXPECT_EQ(config.get<std::string>("inputFile"), "test.fastq");
    EXPECT_EQ(config.get<std::string>("outputDir"), "/tmp/output");
}

TEST_F(ConfigurationTest, GetOrDefault) {
    Configuration config;

    config.set("existingKey", "value");

    EXPECT_EQ(config.getOr<std::string>("existingKey", "default"), "value");
    EXPECT_EQ(config.getOr<std::string>("nonExistingKey", "default"), "default");
    EXPECT_EQ(config.getOr<int64_t>("nonExistingInt", 42), 42);
}

TEST_F(ConfigurationTest, HasKey) {
    Configuration config;

    config.set("existingKey", "value");

    EXPECT_TRUE(config.hasKey("existingKey"));
    EXPECT_FALSE(config.hasKey("nonExistingKey"));
}

TEST_F(ConfigurationTest, GetKeys) {
    Configuration config;

    config.set("key1", "value1");
    config.set("key2", "value2");
    config.set("key3", "value3");

    auto keys = config.keys();
    EXPECT_EQ(keys.size(), 3);
    EXPECT_THAT(keys, testing::UnorderedElementsAre("key1", "key2", "key3"));
}

TEST_F(ConfigurationTest, LoadFromFile) {
    std::string configContent = R"(
# This is a comment
threadCount = 8
batchSize = 1000
debugMode = true
inputFile = "test.fastq"
)";

    createTestConfigFile(configContent);

    Configuration config;
    config.loadFromFile("test_config.txt");

    EXPECT_EQ(config.get<int64_t>("threadCount"), 8);
    EXPECT_EQ(config.get<int64_t>("batchSize"), 1000);
    EXPECT_TRUE(config.get<bool>("debugMode"));
    EXPECT_EQ(config.get<std::string>("inputFile"), "test.fastq");
}

TEST_F(ConfigurationTest, LoadFromFileWithQuotes) {
    std::string configContent = R"(
inputFile = 'test.fastq'
outputDir = "/tmp/output"
description = "This is a test"
)";

    createTestConfigFile(configContent);

    Configuration config;
    config.loadFromFile("test_config.txt");

    EXPECT_EQ(config.get<std::string>("inputFile"), "test.fastq");
    EXPECT_EQ(config.get<std::string>("outputDir"), "/tmp/output");
    EXPECT_EQ(config.get<std::string>("description"), "This is a test");
}

TEST_F(ConfigurationTest, LoadFromArgs) {
    const char* args[] = {"program_name",
                          "--threadCount=8",
                          "--batchSize",
                          "1000",
                          "--debugMode",
                          "--inputFile",
                          "test.fastq"};

    Configuration config;
    config.loadFromArgs(7, const_cast<char**>(args));

    EXPECT_EQ(config.get<int64_t>("threadCount"), 8);
    EXPECT_EQ(config.get<int64_t>("batchSize"), 1000);
    EXPECT_TRUE(config.get<bool>("debugMode"));
    // inputFile 会被解析为字符串，不是int64_t
    EXPECT_EQ(config.get<std::string>("inputFile"), "test.fastq");
}

TEST_F(ConfigurationTest, LoadFromEnv) {
    // 设置环境变量
    setenv("FASTQTOOLS_THREAD_COUNT", "4", 1);
    setenv("FASTQTOOLS_BATCH_SIZE", "500", 1);
    setenv("FASTQTOOLS_DEBUG_MODE", "true", 1);

    Configuration config;
    config.loadFromEnv();

    EXPECT_EQ(config.get<int64_t>("thread_count"), 4);
    EXPECT_EQ(config.get<int64_t>("batch_size"), 500);
    EXPECT_TRUE(config.get<bool>("debug_mode"));

    // 清理环境变量
    unsetenv("FASTQTOOLS_THREAD_COUNT");
    unsetenv("FASTQTOOLS_BATCH_SIZE");
    unsetenv("FASTQTOOLS_DEBUG_MODE");
}

TEST_F(ConfigurationTest, ParseValueTypes) {
    Configuration config;

    // 测试自动类型解析
    config.setFromString("boolTrue", "true");
    config.setFromString("boolFalse", "false");
    config.set<int64_t>("integer", 42);
    config.set<int64_t>("negativeInt", -10);
    config.set<double>("floating", 3.14);
    config.set<std::string>("string", "hello world");

    EXPECT_TRUE(config.get<bool>("boolTrue"));
    EXPECT_FALSE(config.get<bool>("boolFalse"));
    EXPECT_EQ(config.get<int64_t>("integer"), 42);
    EXPECT_EQ(config.get<int64_t>("negativeInt"), -10);
    EXPECT_DOUBLE_EQ(config.get<double>("floating"), 3.14);
    EXPECT_EQ(config.get<std::string>("string"), "hello world");
}

TEST_F(ConfigurationTest, Validation) {
    Configuration config;

    // 设置有效配置
    config.set("input", "in.fq");
    config.set("output", "out.fq");
    config.set("threads", int64_t(8));
    config.set("memoryLimitMb", int64_t(1024));

    // 验证应该通过
    EXPECT_NO_THROW(config.validate());

    // 测试无效配置
    config.set("threads", int64_t(0));
    EXPECT_THROW(config.validate(), fq::error::ConfigurationError);

    config.set("threads", int64_t(8));
    config.set("memoryLimitMb", int64_t(0));
    EXPECT_THROW(config.validate(), fq::error::ConfigurationError);
}

TEST_F(ConfigurationTest, MissingRequiredKey) {
    Configuration config;

    // 只设置部分必需的配置
    config.set("input", "in.fq");
    // 缺少 output

    EXPECT_THROW(config.validate(), fq::error::ConfigurationError);
}

TEST_F(ConfigurationTest, Clear) {
    Configuration config;

    config.set("key1", "value1");
    config.set("key2", "value2");

    EXPECT_EQ(config.size(), 2);
    EXPECT_FALSE(config.empty());

    config.clear();

    EXPECT_EQ(config.size(), 0);
    EXPECT_TRUE(config.empty());
}

TEST_F(ConfigurationTest, InvalidKeyFormat) {
    Configuration config;

    // 测试无效的键名
    EXPECT_THROW(config.set("invalid key", "value"), fq::error::ConfigurationError);
    EXPECT_THROW(config.set("key@with#symbols", "value"), fq::error::ConfigurationError);
    EXPECT_THROW(config.set("", "value"), fq::error::ConfigurationError);
}

TEST_F(ConfigurationTest, WrongTypeAccess) {
    Configuration config;

    config.set("number", "42");

    // 字符串不能作为bool访问
    EXPECT_THROW((void)config.get<bool>("number"), fq::error::ConfigurationError);
}

TEST_F(ConfigurationTest, GlobalConfig) {
    auto& config1 = globalConfig();
    auto& config2 = globalConfig();

    EXPECT_EQ(&config1, &config2);  // 应该是同一个实例

    config1.set("globalKey", "globalValue");
    EXPECT_TRUE(config2.hasKey("globalKey"));
    EXPECT_EQ(config2.get<std::string>("globalKey"), "globalValue");
}

}  // namespace fq::config
