#include <memory>
#include <string>
#include <vector>

#include "common_options.h"
#include <cxxopts.hpp>

#include "cli/filter_plan.h"
#include <fqtools/fq.h>
#include <gtest/gtest.h>

namespace fq::cli {
namespace {

class CapturingPipeline : public fq::processing::ProcessingPipelineInterface {
public:
    void setInputPath(const std::string& inputPath) override {
        inputPath_ = inputPath;
    }

    void setOutputPath(const std::string& outputPath) override {
        outputPath_ = outputPath;
    }

    void setReader(std::unique_ptr<fq::io::IReader> /*reader*/) override {}

    void setWriter(std::unique_ptr<fq::io::IWriter> /*writer*/) override {}

    void setProcessingOptions(const fq::processing::ProcessingOptions& options) override {
        options_ = options;
    }

    void addReadMutator(std::unique_ptr<fq::processing::ReadMutatorInterface> mutator) override {
        mutators_.push_back(std::move(mutator));
    }

    void addReadPredicate(
        std::unique_ptr<fq::processing::ReadPredicateInterface> predicate) override {
        predicates_.push_back(std::move(predicate));
    }

    auto run() -> fq::processing::ProcessingStatistics override {
        return {};
    }

    std::string inputPath_;
    std::string outputPath_;
    fq::processing::ProcessingOptions options_;
    std::vector<std::unique_ptr<fq::processing::ReadPredicateInterface>> predicates_;
    std::vector<std::unique_ptr<fq::processing::ReadMutatorInterface>> mutators_;
};

}  // namespace

TEST(FilterPlanTest, BuildsAndAppliesRepresentativeFilterOptions) {
    cxxopts::Options options("filter", "Filter and trim FastQ files");
    CommonCliOptions::addOptions(options);
    addFilterPlanOptions(options);

    const std::vector<std::string> args = {
        "filter",
        "--input",
        "input.fastq",
        "--output",
        "output.fastq",
        "--threads",
        "2",
        "--batch-size",
        "3",
        "--min-length",
        "4",
        "--trim-quality",
        "20",
        "--trim-mode",
        "three",
        "--adapter-seq",
        "TTAA",
        "--trim-poly-g",
        "4",
    };

    std::vector<char*> argv;
    argv.reserve(args.size());
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }

    const auto parsed = options.parse(static_cast<int>(argv.size()), argv.data());
    const auto common = CommonCliOptions::parse(parsed);

    auto plan = buildFilterPlan(parsed, common);
    CapturingPipeline pipeline;
    plan.applyTo(pipeline);

    EXPECT_EQ(pipeline.inputPath_, "input.fastq");
    EXPECT_EQ(pipeline.outputPath_, "output.fastq");
    EXPECT_EQ(pipeline.options_.threadCount, 2U);
    EXPECT_EQ(pipeline.options_.batchSize, 3U);

    ASSERT_EQ(pipeline.predicates_.size(), 1U);
    EXPECT_NE(dynamic_cast<fq::processing::MinLengthPredicate*>(pipeline.predicates_[0].get()),
              nullptr);

    ASSERT_EQ(pipeline.mutators_.size(), 3U);
    EXPECT_NE(dynamic_cast<fq::processing::QualityTrimmer*>(pipeline.mutators_[0].get()), nullptr);
    EXPECT_NE(dynamic_cast<fq::processing::AdapterTrimmer*>(pipeline.mutators_[1].get()), nullptr);
    EXPECT_NE(dynamic_cast<fq::processing::PolyTailTrimmer*>(pipeline.mutators_[2].get()), nullptr);

    fq::io::FastqRecord read{"read1", {}, "ACGT", "II!!", "+"};
    pipeline.mutators_[0]->process(read);
    EXPECT_EQ(read.seq, "AC");
    EXPECT_EQ(read.qual, "II");
}

}  // namespace fq::cli
