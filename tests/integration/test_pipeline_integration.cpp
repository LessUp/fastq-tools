#include <filesystem>
#include <fstream>
#include <string>

#include <fqtools/fq.h>

#include "fixture_loader.h"
#include "test_helpers.h"

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

}  // namespace fq::test
