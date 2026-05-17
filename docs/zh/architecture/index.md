# 架构

架构页是白皮书从“定位叙事”进入“系统模型”的地方。这里真正要回答的，不只是“有哪些模块”，而是“哪些边界让 FastQTools 在进入实现评审时仍然可解释、可复核”。

## 系统分层

FastQTools 把 CLI、命令编排、公共 API 与具体实现刻意分开，让不同读者始终只需要面对一个稳定表面。

- **CLI 与命令层** 负责参数解析、任务编排、日志与面向终端的错误边界；
- **公共 API 层** 把 `include/fqtools/` 保持为 CLI、测试与外部 C++ 调用方共享的集成面；
- **实现层** 承担 I/O、处理、统计、配置与错误管理，是吞吐和正确性真正落地的地方。

<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge> 定义的正是这套分层：它让 FastQTools 可以先通过 CLI 做采用判断，再通过 API 与实现材料做技术审查，而不会在切换阅读视角时丢失同一套系统心智模型。也因此，本页之后最自然的下一站是 [`算法`](../algorithms/)，而不是杂乱的类名索引。

<DiagramFrame
  asset="architecture-overview"
  caption="架构总览：入口、公共 API 与执行路径围绕同一套白皮书叙事组织。"
/>

## 执行模型

维护中的执行叙事仍然是 `source → processing → sink`，但架构关注的是“控制权在哪里交接”：

1. **source** 负责把普通 FASTQ 或 gzip FASTQ 读成有界批次；
2. **processing** 负责把维护中的 predicate、mutator 与统计逻辑作用到记录视图上；
3. **sink** 负责写回过滤结果或统计输出，稳定 I/O 与资源回收边界。

这也是为什么需要 [`算法`](../algorithms/) 这一层：本页说明“为什么这些阶段边界必须存在”，算法页再说明“这些阶段今天具体做什么”。真正的架构主张不是“项目用了并行”，而是它把并行放进了可以解释、可以验证的阶段边界里。

## 关键权衡

FastQTools 最关键的架构权衡之一，是把内存所有权当成一条一等边界来处理：

- `FastqBatch` 持有连续存储，减少逐条记录复制；
- `std::string_view` 让遍历保持轻量，但前提是批次生命周期绝不能被破坏；
- 对象池复用控制分配抖动，但前提是 in-flight 批次数量与复用时机始终受控。

这正是 <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md">RFC-0004</ReferenceBadge> 的重点。也因此，白皮书可以把“吞吐”和“内存有界”写在同一套叙事里，而不会沦为口号。

## 验证梯度

仅有架构并不能证明系统可靠，但它能告诉你下一步该去哪里验证：

- 回到 [`白皮书`](../whitepaper/)：补足产品边界与评审语境；
- 继续看 [`算法`](../algorithms/)：检查过滤、统计、执行与内存行为如何落成维护中的实现策略；
- 再看 [`性能总览`](../performance/)：核对这些边界是否真的反映到 benchmark 证据里；
- 最后看 [`研究附录 / 演进说明`](../research/evolution-notes)：理解这些边界为什么被保留下来。

## 继续下钻

如果你想把这些结构直接和测量结果对应起来，下一步看 [`性能总览`](../performance/)；如果你想理解这些阶段今天具体执行哪些受支持行为，下一步看 [`算法`](../algorithms/)；如果你想直接进入实现与代码契约，再继续看 [`开发者架构设计`](../dev/architecture)、[`核心设计`](../dev/design) 与 [`IO 模块`](../api/io)。
