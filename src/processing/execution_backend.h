#pragma once

#include "fqtools/io/interfaces.h"

#include <any>
#include <cstddef>
#include <cstdint>
#include <memory>

#include "processing/resolved_runtime_config.h"

namespace fq::processing {

struct ExecutionRuntimeMetrics {
    std::uint64_t batchCount = 0;
    std::uint64_t committedBytes = 0;
};

struct ErasedExecutionOutcome {
    std::any result;
    ExecutionRuntimeMetrics metrics;
};

/**
 * @brief 批处理操作的类型擦除 seam
 * @details 每批仅进行一次虚调用与结果装箱，逐 read 热点不经过该接口。
 */
class ExecutionOperation {
public:
    virtual ~ExecutionOperation() = default;

    [[nodiscard]] virtual auto makeResult() -> std::any = 0;
    virtual auto processBatch(fq::io::FastqBatch& batch) -> std::any = 0;
    virtual void afterCommit(std::any& partial, std::uint64_t committedBytes) = 0;
    virtual void merge(std::any& total, std::any partial) = 0;
};

struct ExecutionBackendContext {
    std::shared_ptr<fq::io::IReader> reader;
    std::shared_ptr<fq::io::IWriter> writer;
    ResolvedRuntimeConfig config;
    size_t batchSize = 1;
};

/**
 * @brief 执行后端 seam
 * @details 隐藏具体调度框架，所有后端共享相同 I/O 与批处理契约。
 */
class ExecutionBackend {
public:
    virtual ~ExecutionBackend() = default;

    [[nodiscard]] virtual auto execute(ExecutionBackendContext context,
                                       ExecutionOperation& operation) -> ErasedExecutionOutcome = 0;
};

[[nodiscard]] auto createSequentialExecutionBackend() -> std::unique_ptr<ExecutionBackend>;
[[nodiscard]] auto createOneTbbExecutionBackend() -> std::unique_ptr<ExecutionBackend>;
[[nodiscard]] auto createTaskflowExecutionBackend() -> std::unique_ptr<ExecutionBackend>;
[[nodiscard]] auto isTaskflowExecutionBackendAvailable() noexcept -> bool;

}  // namespace fq::processing
