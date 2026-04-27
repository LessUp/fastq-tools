#include <filesystem>
#include <fstream>
#include <string>

#include "fixture_loader.h"
#include "test_helpers.h"

#include <fqtools/fq.h>
#include <gtest/gtest.h>

namespace fq::test {

class PipelineIntegrationTest : public FastQToolsTest {};

TEST_F(PipelineIntegrationTest, ReaderPipelineWriterPreservesCustomPlusLine) {
    const auto input = tempDir_.path() / "input.fastq";
    const auto output = tempDir_.path() / "output.fastq";

    {
        std::ofstream out(input);
        out << "@read1 comment\n"
            << "ACGT\n"
            << "+read1 comment\n"
            << "IIII\n"
            << "@read2\n"
            << "TTTT\n"
            << "+\n"
            << "####\n";
    }

    auto pipeline = fq::processing::createProcessingPipeline();
    pipeline->setInputPath(input.string());
    pipeline->setOutputPath(output.string());

    fq::processing::ProcessingConfig config;
    config.threadCount = 1;
    config.batchSize = 2;
    config.batchCapacityBytes = 1024;
    pipeline->setProcessingConfig(config);

    const auto stats = pipeline->run();

    EXPECT_EQ(stats.totalReads, 2);
    EXPECT_EQ(stats.passedReads, 2);

    const auto content = FixtureLoader::loadTextFile(output);
    EXPECT_NE(content.find("@read1 comment\nACGT\n+read1 comment\nIIII\n"), std::string::npos);
    EXPECT_NE(content.find("@read2\nTTTT\n+\n####\n"), std::string::npos);
}

TEST_F(PipelineIntegrationTest, StatisticCalculatorWritesConfiguredPhredHeader) {
    const auto input = tempDir_.path() / "input.fastq";
    const auto output = tempDir_.path() / "stats.txt";

    {
        std::ofstream out(input);
        out << "@read1\n"
            << "A\n"
            << "+\n"
            << "@\n";
    }

    fq::statistic::StatisticOptions options;
    options.inputFastqPath = input.string();
    options.outputStatPath = output.string();
    options.batchSize = 1;
    options.threadCount = 1;
    options.qualityEncoding = 64;

    auto calculator = fq::statistic::createStatisticCalculator(options);
    calculator->run();

    const auto content = FixtureLoader::loadTextFile(output);
    EXPECT_NE(content.find("#PhredQual\t64\n"), std::string::npos);
    EXPECT_NE(content.find("#ReadNum\t1\n"), std::string::npos);
}

TEST_F(PipelineIntegrationTest, PipelineRunsWithExplicitOneTbbBackend) {
    const auto input = tempDir_.path() / "input.fastq";
    const auto output = tempDir_.path() / "output.fastq";

    {
        std::ofstream out(input);
        out << "@read1\n"
            << "ACGT\n"
            << "+\n"
            << "IIII\n"
            << "@read2\n"
            << "TTTT\n"
            << "+\n"
            << "IIII\n";
    }

    auto pipeline = fq::processing::createProcessingPipeline();
    pipeline->setInputPath(input.string());
    pipeline->setOutputPath(output.string());

    fq::processing::ProcessingConfig config;
    config.threadCount = 2;
    config.batchSize = 1;
    config.batchCapacityBytes = 1024;
    config.executionBackend = fq::processing::ExecutionBackend::OneTbb;
    pipeline->setProcessingConfig(config);

    const auto stats = pipeline->run();

    EXPECT_EQ(stats.totalReads, 2);
    EXPECT_EQ(stats.passedReads, 2);
    EXPECT_TRUE(std::filesystem::exists(output));
}

TEST_F(PipelineIntegrationTest, StatisticCalculatorRunsWithExplicitOneTbbBackend) {
    const auto input = tempDir_.path() / "input.fastq";
    const auto output = tempDir_.path() / "stats.txt";

    {
        std::ofstream out(input);
        out << "@read1\n"
            << "A\n"
            << "+\n"
            << "I\n"
            << "@read2\n"
            << "T\n"
            << "+\n"
            << "I\n";
    }

    fq::statistic::StatisticOptions options;
    options.inputFastqPath = input.string();
    options.outputStatPath = output.string();
    options.batchSize = 1;
    options.threadCount = 2;
    options.executionBackend = fq::processing::ExecutionBackend::OneTbb;

    auto calculator = fq::statistic::createStatisticCalculator(options);
    calculator->run();

    const auto content = FixtureLoader::loadTextFile(output);
    EXPECT_NE(content.find("#ReadNum\t2\n"), std::string::npos);
}

TEST_F(PipelineIntegrationTest, PipelineReportsMemoryTelemetryWhenEnabled) {
    const auto input = tempDir_.path() / "input.fastq";
    const auto output = tempDir_.path() / "output.fastq";

    {
        std::ofstream out(input);
        out << "@read1\n"
            << "ACGT\n"
            << "+\n"
            << "IIII\n";
    }

    auto pipeline = fq::processing::createProcessingPipeline();
    pipeline->setInputPath(input.string());
    pipeline->setOutputPath(output.string());

    fq::processing::ProcessingConfig config;
    config.threadCount = 2;
    config.batchSize = 1;
    config.batchCapacityBytes = 1024;
    config.maxInFlightBatches = 3;
    config.memoryResourcePolicy = fq::processing::MemoryResourcePolicy::ObjectPool;
    config.allocationTelemetryEnabled = true;
    pipeline->setProcessingConfig(config);

    const auto stats = pipeline->run();

    EXPECT_TRUE(stats.allocationTelemetryEnabled);
    EXPECT_EQ(stats.memoryResourcePolicy, fq::processing::MemoryResourcePolicy::ObjectPool);
    EXPECT_EQ(stats.resolvedMaxInFlightBatches, 3u);
}

TEST_F(PipelineIntegrationTest, StatisticCalculatorWritesMemoryTelemetryWhenEnabled) {
    const auto input = tempDir_.path() / "input.fastq";
    const auto output = tempDir_.path() / "stats.txt";

    {
        std::ofstream out(input);
        out << "@read1\n"
            << "A\n"
            << "+\n"
            << "I\n";
    }

    fq::statistic::StatisticOptions options;
    options.inputFastqPath = input.string();
    options.outputStatPath = output.string();
    options.batchSize = 1;
    options.threadCount = 2;
    options.maxInFlightBatches = 3;
    options.memoryResourcePolicy = fq::processing::MemoryResourcePolicy::ObjectPool;
    options.allocationTelemetryEnabled = true;

    auto calculator = fq::statistic::createStatisticCalculator(options);
    calculator->run();

    const auto content = FixtureLoader::loadTextFile(output);
    EXPECT_NE(content.find("#MemoryPolicy\tobjectPool\n"), std::string::npos);
    EXPECT_NE(content.find("#MaxInFlightBatches\t3\n"), std::string::npos);
}

}  // namespace fq::test
