# 架构

本页帮助你建立 FastQTools 的系统心智模型：哪些层负责入口，哪些层负责吞吐，哪些约束让性能主张保持可解释，哪些资源边界保证它不只是一个“看起来很快”的口号。

## 系统分层

FastQTools 采用 CLI / 命令编排 / 公共 API / 具体实现的分层结构，让终端入口、库接口与内部实现保持清晰边界。

- **CLI 与命令层** 负责参数解析、任务编排、日志与错误边界，确保终端入口不会直接耦合到底层实现细节；
- **公共 API 层** 通过 `include/fqtools/` 暴露稳定接口，让 CLI、测试和外部 C++ 集成都依赖同一套契约；
- **实现层** 承担 I/O、处理、统计、配置和错误管理，是性能、正确性与资源有界性的落点。

这套分层的关键不只是“模块名好看”，而是让采用者能回答：我是在使用一个一次性命令集合，还是一个可以先 CLI 验证、再逐步嵌入程序的工程内核？如果需要继续下钻，可看 [`API 概览`](../api/overview)、[`开发者架构设计`](../dev/architecture) 与 [`核心设计`](../dev/design)。

<DiagramFrame
  asset="architecture-overview"
  caption="架构总览：入口、公共 API 与执行路径围绕同一套白皮书叙事组织。"
/>

## 执行模型

系统把数据流组织为 `source → processing → sink`：读取与写回保持边界清晰，真正适合并行的过滤、裁剪和统计逻辑放在处理中段。

其中最值得注意的不是“用了并行”，而是**怎样并行、边界在哪里**：

1. **source** 阶段把输入文件读取成有界批次；`FastqBatch` 持有连续缓冲区，避免把每条记录都复制成独立字符串；
2. **processing** 阶段让记录通过 `std::string_view` 暴露视图，把过滤、裁剪、统计等热点工作放进可并行的处理中段；
3. **sink** 阶段再把结果写回文件或汇总输出，保持 I/O 顺序、错误处理与资源回收边界清晰。

这意味着 FastQTools 不是“读一行、拷一份、改一份”的串行命令封装，而是在批次生命周期内尽量减少复制，再用 staged pipeline 把吞吐和可解释性放到同一模型里。相关的设计依据可继续追到 [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md) 与 [`RFC-0004: Memory Pool Optimization`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md)。

## 关键权衡

- 用零拷贝批处理模型减少无谓复制，但必须严格维护 `std::string_view` 生命周期
- 用并行流水线换取更高吞吐，但保持资源有界与输出可解释
- 用公共 API 隔离 CLI 与实现细节，降低后续集成与维护成本

这些权衡背后对应的是一组明确的正确性边界：

- **生命周期正确性**：`std::string_view` 不能逃逸出所属批次；批次复用前必须完成所有仍依赖该缓冲区的处理；
- **资源有界**：in-flight 批次、缓冲区复用和并行度都必须受控，避免文件越大、内存越失控；
- **输出可解释**：source 与 sink 负责稳定入口/出口边界，中段并行不能破坏同配置下的结果语义。

如果你需要把这些主张与长期维护约束对应起来，可回看：

- [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
- [`RFC-0002: Toolchain Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)
- [`RFC-0004: Memory Pool Optimization`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md)

## 继续下钻

如果你想把这些结构与证据对应起来，下一步看 [`性能总览`](../performance/)；如果你想按角色进入实操材料，下一步看 [`工作流`](../workflows/)；如果你想直接审查实现细节，再继续看 [`开发者架构设计`](../dev/architecture) 与 [`IO 模块`](../api/io)。
