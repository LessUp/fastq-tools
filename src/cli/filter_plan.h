#pragma once

#include <memory>
#include <vector>

#include "common_options.h"
#include <cxxopts.hpp>

#include <fqtools/processing/mutators.h>
#include <fqtools/processing/predicates.h>
#include <fqtools/processing/processing_pipeline_interface.h>

namespace fq::cli {

struct FilterPlan {
    std::string inputPath;
    std::string outputPath;
    fq::processing::ProcessingOptions processingOptions;
    std::vector<std::unique_ptr<fq::processing::ReadPredicateInterface>> predicates;
    std::vector<std::unique_ptr<fq::processing::ReadMutatorInterface>> mutators;

    auto applyTo(fq::processing::ProcessingPipelineInterface& pipeline) -> void;
};

void addFilterPlanOptions(cxxopts::Options& options);

[[nodiscard]] auto buildFilterPlan(const cxxopts::ParseResult& result,
                                   const CommonCliOptions& common) -> FilterPlan;

}  // namespace fq::cli
