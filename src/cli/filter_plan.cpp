#include "filter_plan.h"

#include <string>
#include <vector>

namespace fq::cli {

namespace {

auto parseTrimMode(const std::string& mode) -> fq::processing::QualityTrimmer::TrimMode {
    if (mode == "five") {
        return fq::processing::QualityTrimmer::TrimMode::FivePrime;
    }
    if (mode == "three") {
        return fq::processing::QualityTrimmer::TrimMode::ThreePrime;
    }
    return fq::processing::QualityTrimmer::TrimMode::Both;
}

}  // namespace

auto FilterPlan::applyTo(fq::processing::ProcessingPipelineInterface& pipeline) -> void {
    pipeline.setInputPath(inputPath);
    pipeline.setOutputPath(outputPath);
    pipeline.setProcessingOptions(processingOptions);

    for (auto& predicate : predicates) {
        pipeline.addReadPredicate(std::move(predicate));
    }

    for (auto& mutator : mutators) {
        pipeline.addReadMutator(std::move(mutator));
    }
}

void addFilterPlanOptions(cxxopts::Options& options) {
    options.add_options()("quality-encoding",
                          "Quality encoding offset (33 or 64)",
                          cxxopts::value<int>()->default_value("33"))(
        "min-quality", "Minimum average quality threshold", cxxopts::value<double>())(
        "min-length", "Minimum read length", cxxopts::value<size_t>())(
        "max-length", "Maximum read length", cxxopts::value<size_t>())(
        "max-n-ratio", "Maximum N ratio (0.0-1.0)", cxxopts::value<double>())(
        "trim-quality", "Trim bases below quality threshold", cxxopts::value<double>())(
        "trim-mode",
        "Trim mode (both,five,three)",
        cxxopts::value<std::string>()->default_value("both"))(
        "adapter-seq",
        "Trim adapter sequence from 3' end (repeatable)",
        cxxopts::value<std::vector<std::string>>())("adapter-min-overlap",
                                                    "Minimum adapter overlap",
                                                    cxxopts::value<size_t>()->default_value("3"))(
        "adapter-max-mismatches",
        "Maximum adapter mismatches",
        cxxopts::value<size_t>()->default_value("1"))(
        "trim-poly-g", "Trim polyG tail with minimum run length", cxxopts::value<size_t>())(
        "trim-poly-x",
        "Trim low-complexity polyX tail with minimum run length",
        cxxopts::value<size_t>());
}

auto buildFilterPlan(const cxxopts::ParseResult& result,
                     const CommonCliOptions& common) -> FilterPlan {
    FilterPlan plan;
    plan.inputPath = common.inputPath;
    plan.outputPath = common.outputPath;
    plan.processingOptions = common.toProcessingOptions();

    const int qualityEncoding = result["quality-encoding"].as<int>();

    if (result.count("min-quality")) {
        plan.predicates.push_back(std::make_unique<fq::processing::MinQualityPredicate>(
            result["min-quality"].as<double>(), qualityEncoding));
    }

    if (result.count("min-length")) {
        plan.predicates.push_back(std::make_unique<fq::processing::MinLengthPredicate>(
            result["min-length"].as<size_t>()));
    }

    if (result.count("max-length")) {
        plan.predicates.push_back(std::make_unique<fq::processing::MaxLengthPredicate>(
            result["max-length"].as<size_t>()));
    }

    if (result.count("max-n-ratio")) {
        plan.predicates.push_back(std::make_unique<fq::processing::MaxNRatioPredicate>(
            result["max-n-ratio"].as<double>()));
    }

    if (result.count("trim-quality")) {
        plan.mutators.push_back(std::make_unique<fq::processing::QualityTrimmer>(
            result["trim-quality"].as<double>(),
            static_cast<size_t>(1),
            parseTrimMode(result["trim-mode"].as<std::string>()),
            qualityEncoding));
    }

    if (result.count("adapter-seq")) {
        plan.mutators.push_back(std::make_unique<fq::processing::AdapterTrimmer>(
            result["adapter-seq"].as<std::vector<std::string>>(),
            result["adapter-min-overlap"].as<size_t>(),
            result["adapter-max-mismatches"].as<size_t>()));
    }

    if (result.count("trim-poly-g")) {
        plan.mutators.push_back(std::make_unique<fq::processing::PolyTailTrimmer>(
            fq::processing::PolyTailTrimmer::TailKind::PolyG, result["trim-poly-g"].as<size_t>()));
    }

    if (result.count("trim-poly-x")) {
        plan.mutators.push_back(std::make_unique<fq::processing::PolyTailTrimmer>(
            fq::processing::PolyTailTrimmer::TailKind::PolyX, result["trim-poly-x"].as<size_t>()));
    }

    return plan;
}

}  // namespace fq::cli
