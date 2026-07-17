#include <utility>

#include "processing/processing_pipeline.h"

namespace fq::processing {

class Pipeline::Impl {
public:
    ProcessingPipeline pipeline;
};

Pipeline::Pipeline() : impl_(std::make_unique<Impl>()) {}
Pipeline::~Pipeline() = default;
Pipeline::Pipeline(Pipeline&&) noexcept = default;
auto Pipeline::operator=(Pipeline&&) noexcept -> Pipeline& = default;

void Pipeline::setInputPath(const std::string& inputPath) {
    impl_->pipeline.setInputPath(inputPath);
}

void Pipeline::setOutputPath(const std::string& outputPath) {
    impl_->pipeline.setOutputPath(outputPath);
}

void Pipeline::setReader(std::unique_ptr<fq::io::IReader> reader) {
    impl_->pipeline.setReader(std::move(reader));
}

void Pipeline::setWriter(std::unique_ptr<fq::io::IWriter> writer) {
    impl_->pipeline.setWriter(std::move(writer));
}

void Pipeline::setProcessingOptions(const ProcessingOptions& options) {
    impl_->pipeline.setProcessingOptions(options);
}

void Pipeline::addReadMutator(std::unique_ptr<ReadMutatorInterface> mutator) {
    impl_->pipeline.addReadMutator(std::move(mutator));
}

void Pipeline::addReadPredicate(std::unique_ptr<ReadPredicateInterface> predicate) {
    impl_->pipeline.addReadPredicate(std::move(predicate));
}

auto Pipeline::run() -> ProcessingStatistics {
    return impl_->pipeline.run();
}

}  // namespace fq::processing
