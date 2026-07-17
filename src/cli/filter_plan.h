#pragma once

#include <memory>
#include <utility>
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

    template <typename PipelineLike>
    void applyTo(PipelineLike& pipeline) {
        pipeline.setInputPath(inputPath);
        pipeline.setOutputPath(outputPath);
        pipeline.setProcessingOptions(processingOptions);

        for (auto& mutator : mutators) {
            pipeline.addReadMutator(std::move(mutator));
        }

        for (auto& predicate : predicates) {
            pipeline.addReadPredicate(std::move(predicate));
        }
    }
};

void addFilterPlanOptions(cxxopts::Options& options);

[[nodiscard]] auto buildFilterPlan(const cxxopts::ParseResult& result,
                                   const CommonCliOptions& common) -> FilterPlan;

}  // namespace fq::cli
