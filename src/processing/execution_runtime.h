#pragma once

#include "fqtools/io/fastq_io.h"
#include "fqtools/processing/processing_options.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <utility>

namespace fq::processing {

struct ExecutionRuntimePlan {
    std::string inputPath;
    std::optional<std::string> outputPath;
    ProcessingOptions options;
};

class ExecutionRuntimeAdapter {
public:
    virtual ~ExecutionRuntimeAdapter() = default;
    virtual auto nextBatch(fq::io::FastqBatch& batch, size_t maxRecords) -> bool = 0;
    virtual auto commit(const fq::io::FastqBatch& batch) -> std::uint64_t = 0;
};

class ExecutionRuntime {
public:
    explicit ExecutionRuntime(std::unique_ptr<ExecutionRuntimeAdapter> adapter);

    template <typename Final, typename BatchWork, typename Reduce, typename AfterCommit>
    auto run(const ExecutionRuntimePlan& plan,
             BatchWork&& batchWork,
             Reduce&& reduce,
             AfterCommit&& afterCommit,
             Final initial) -> Final {
        plan.options.validate();
        fq::io::FastqBatch batch;
        Final finalResult = std::move(initial);
        while (adapter_->nextBatch(batch, plan.options.batchSize)) {
            auto partial = batchWork(batch);
            const auto committedBytes = adapter_->commit(batch);
            afterCommit(partial, committedBytes);
            reduce(finalResult, std::move(partial));
        }
        return finalResult;
    }

private:
    std::unique_ptr<ExecutionRuntimeAdapter> adapter_;
};

}  // namespace fq::processing
