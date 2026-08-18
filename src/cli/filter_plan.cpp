#include "filter_plan.h"

#include <stdexcept>
#include <string>
#include <vector>

#include "enum_parser.h"

namespace fq::cli {

namespace {

void validateNonNegativeThreshold(const char* name, double value) {
    if (value < 0.0) {
        throw std::invalid_argument(std::string(name) + " must be >= 0");
    }
}

void validateMaxNRatio(double value) {
    if (value < 0.0 || value > 1.0) {
        throw std::invalid_argument("max-n-ratio must be between 0.0 and 1.0");
    }
}

void validateLengthBounds(const cxxopts::ParseResult& result) {
    if (!result.count("min-length") || !result.count("max-length")) {
        return;
    }

    const size_t minLength = result["min-length"].as<size_t>();
    const size_t maxLength = result["max-length"].as<size_t>();
    if (minLength > maxLength) {
        throw std::invalid_argument("min-length must be <= max-length");
    }
}

}  // namespace

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
        cxxopts::value<size_t>())("trim-length",
                                  "Trim reads to this length from the 3' end (keep 5' prefix)",
                                  cxxopts::value<size_t>());
}

auto buildFilterPlan(const cxxopts::ParseResult& result, const CommonCliOptions& common)
    -> FilterPlan {
    FilterPlan plan;
    plan.inputPath = common.inputPath;
    plan.outputPath = common.outputPath;
    plan.processingOptions = common.toProcessingOptions();

    validateLengthBounds(result);

    const int qualityEncoding = validateQualityEncoding(result["quality-encoding"].as<int>());

    if (result.count("min-length")) {
        plan.predicates.push_back(std::make_unique<fq::processing::MinLengthPredicate>(
            result["min-length"].as<size_t>()));
    }

    if (result.count("max-length")) {
        plan.predicates.push_back(std::make_unique<fq::processing::MaxLengthPredicate>(
            result["max-length"].as<size_t>()));
    }

    if (result.count("min-quality")) {
        const double minQuality = result["min-quality"].as<double>();
        validateNonNegativeThreshold("min-quality", minQuality);
        plan.predicates.push_back(
            std::make_unique<fq::processing::MinQualityPredicate>(minQuality, qualityEncoding));
    }

    if (result.count("max-n-ratio")) {
        const double maxNRatio = result["max-n-ratio"].as<double>();
        validateMaxNRatio(maxNRatio);
        plan.predicates.push_back(std::make_unique<fq::processing::MaxNRatioPredicate>(maxNRatio));
    }

    // CLI 契约：adapter → poly-G/poly-X → quality trim → length trim，所有 predicate 在其后评估。
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

    if (result.count("trim-quality")) {
        const double trimQuality = result["trim-quality"].as<double>();
        validateNonNegativeThreshold("trim-quality", trimQuality);
        plan.mutators.push_back(std::make_unique<fq::processing::QualityTrimmer>(
            trimQuality,
            static_cast<size_t>(1),
            parseTrimMode(result["trim-mode"].as<std::string>()),
            qualityEncoding));
    }

    if (result.count("trim-length")) {
        const size_t trimLength = result["trim-length"].as<size_t>();
        if (trimLength == 0) {
            throw std::invalid_argument("trim-length must be >= 1");
        }
        plan.mutators.push_back(std::make_unique<fq::processing::LengthTrimmer>(
            trimLength, fq::processing::LengthTrimmer::TrimStrategy::MaxLength));
    }

    return plan;
}

}  // namespace fq::cli
