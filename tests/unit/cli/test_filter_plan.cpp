#include <memory>
#include <stdexcept>
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
    EXPECT_NE(dynamic_cast<fq::processing::AdapterTrimmer*>(pipeline.mutators_[0].get()), nullptr);
    EXPECT_NE(dynamic_cast<fq::processing::PolyTailTrimmer*>(pipeline.mutators_[1].get()), nullptr);
    EXPECT_NE(dynamic_cast<fq::processing::QualityTrimmer*>(pipeline.mutators_[2].get()), nullptr);

    fq::io::FastqRecord read{"read1", {}, "ACGT", "II!!", "+"};
    pipeline.mutators_[2]->process(read);
    EXPECT_EQ(read.seq, "AC");
    EXPECT_EQ(read.qual, "II");
}

TEST(FilterPlanTest, BuildsAdapterAndPolyXTailMutatorsFromRepeatableOptions) {
    cxxopts::Options options("filter", "Filter and trim FastQ files");
    CommonCliOptions::addOptions(options);
    addFilterPlanOptions(options);

    const std::vector<std::string> args = {
        "filter",
        "--input",
        "input.fastq",
        "--output",
        "output.fastq",
        "--adapter-seq",
        "TTAA",
        "--adapter-seq",
        "GGCC",
        "--adapter-min-overlap",
        "4",
        "--adapter-max-mismatches",
        "1",
        "--trim-poly-x",
        "5",
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

    ASSERT_EQ(pipeline.mutators_.size(), 2U);

    auto* const adapterTrimmer =
        dynamic_cast<fq::processing::AdapterTrimmer*>(pipeline.mutators_[0].get());
    ASSERT_NE(adapterTrimmer, nullptr);

    fq::io::FastqRecord read1{"read1", {}, "ACGTTTAA", "IIIIIIII", "+"};
    adapterTrimmer->process(read1);
    EXPECT_EQ(read1.seq, "ACGT");
    EXPECT_EQ(read1.qual, "IIII");

    fq::io::FastqRecord read2{"read2", {}, "ACGTGGCC", "IIIIIIII", "+"};
    adapterTrimmer->process(read2);
    EXPECT_EQ(read2.seq, "ACGT");
    EXPECT_EQ(read2.qual, "IIII");

    auto* const polyTailTrimmer =
        dynamic_cast<fq::processing::PolyTailTrimmer*>(pipeline.mutators_[1].get());
    ASSERT_NE(polyTailTrimmer, nullptr);

    fq::io::FastqRecord polyXRead{"polyX", {}, "ACGTAAAAA", "IIIIIIIII", "+"};
    polyTailTrimmer->process(polyXRead);
    EXPECT_EQ(polyXRead.seq, "ACGT");
    EXPECT_EQ(polyXRead.qual, "IIII");

    fq::io::FastqRecord shortPolyXRead{"shortPolyX", {}, "ACGTTTT", "IIIIIII", "+"};
    polyTailTrimmer->process(shortPolyXRead);
    EXPECT_EQ(shortPolyXRead.seq, "ACGTTTT");
    EXPECT_EQ(shortPolyXRead.qual, "IIIIIII");
}

TEST(FilterPlanTest, RejectsUnsupportedQualityEncoding) {
    cxxopts::Options options("filter", "Filter and trim FastQ files");
    CommonCliOptions::addOptions(options);
    addFilterPlanOptions(options);

    const std::vector<std::string> args = {
        "filter", "--input", "input.fastq", "--output", "output.fastq", "--quality-encoding", "40"};

    std::vector<char*> argv;
    argv.reserve(args.size());
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }

    const auto parsed = options.parse(static_cast<int>(argv.size()), argv.data());
    const auto common = CommonCliOptions::parse(parsed);

    EXPECT_THROW(static_cast<void>(buildFilterPlan(parsed, common)), std::invalid_argument);
}

TEST(FilterPlanTest, RejectsOutOfRangeMaxNRatioAndConflictingLengths) {
    cxxopts::Options options("filter", "Filter and trim FastQ files");
    CommonCliOptions::addOptions(options);
    addFilterPlanOptions(options);

    {
        const std::vector<std::string> args = {
            "filter", "--input", "input.fastq", "--output", "output.fastq", "--max-n-ratio", "1.5"};

        std::vector<char*> argv;
        argv.reserve(args.size());
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }

        const auto parsed = options.parse(static_cast<int>(argv.size()), argv.data());
        const auto common = CommonCliOptions::parse(parsed);

        EXPECT_THROW(static_cast<void>(buildFilterPlan(parsed, common)), std::invalid_argument);
    }

    {
        const std::vector<std::string> args = {"filter",
                                               "--input",
                                               "input.fastq",
                                               "--output",
                                               "output.fastq",
                                               "--min-length",
                                               "10",
                                               "--max-length",
                                               "3"};

        std::vector<char*> argv;
        argv.reserve(args.size());
        for (const auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }

        const auto parsed = options.parse(static_cast<int>(argv.size()), argv.data());
        const auto common = CommonCliOptions::parse(parsed);

        EXPECT_THROW(static_cast<void>(buildFilterPlan(parsed, common)), std::invalid_argument);
    }
}

TEST(FilterPlanTest, RejectsNegativeQualityThresholds) {
    cxxopts::Options options("filter", "Filter and trim FastQ files");
    CommonCliOptions::addOptions(options);
    addFilterPlanOptions(options);

    const std::vector<std::string> args = {"filter",
                                           "--input",
                                           "input.fastq",
                                           "--output",
                                           "output.fastq",
                                           "--min-quality",
                                           "-1",
                                           "--trim-quality",
                                           "-2"};

    std::vector<char*> argv;
    argv.reserve(args.size());
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }

    const auto parsed = options.parse(static_cast<int>(argv.size()), argv.data());
    const auto common = CommonCliOptions::parse(parsed);

    EXPECT_THROW(static_cast<void>(buildFilterPlan(parsed, common)), std::invalid_argument);
}

}  // namespace fq::cli
