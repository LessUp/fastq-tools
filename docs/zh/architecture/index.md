# 架构

架构页是白皮书从“定位叙事”进入“系统模型”的地方。这里真正要回答的，不只是“有哪些模块”，而是“哪些边界让 FastQTools 在进入实现评审时仍然可解释、可复核”。

## 系统分层

FastQTools 把 CLI、命令编排、公共 API 与具体实现刻意分开，让不同读者始终只需要面对一个稳定表面。

- **CLI 与命令层** 负责参数解析、任务编排、日志与面向终端的错误边界；
- **公共 API 层** 把 `include/fqtools/` 保持为 CLI、测试与外部 C++ 调用方共享的集成面；
- **实现层** 承担 I/O、处理、统计、配置与错误管理，是吞吐和正确性真正落地的地方。

<ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md">RFC-0001</ReferenceBadge> 定义的正是这套分层：它让 FastQTools 可以先通过 CLI 做采用判断，再通过 API 与实现材料做技术审查，而不会在切换阅读视角时丢失同一套系统心智模型。

<DiagramFrame
  asset="architecture-overview"
  locale="zh"
  caption="架构总览：入口、公共 API 与执行路径围绕同一套白皮书叙事组织。"
/>

## 边界账本

维护中的架构可以被读成一份所有权交接账本：

1. 输入先通过有界 reader，把普通或 gzip FASTQ 归一成可复用批次；
2. 批次再进入 processing core，在记录视图之上执行 predicate、mutator 与统计；
3. 输出随后进入 sink 与 reporting 层，把终端行为、文件产物与证据表面稳定下来。

这份账本的重要性在于，它让评审者能够问“控制权和所有权在哪里交接”，而不是“哪一个文件最近被调过 benchmark”。

## 执行模型

维护中的执行叙事仍然是 `source → processing → sink`，但架构关注的是“控制权在哪里交接”：

1. **source** 负责把普通 FASTQ 或 gzip FASTQ 读成有界批次；
2. **processing** 负责把维护中的 predicate、mutator 与统计逻辑作用到记录视图上；
3. **sink** 负责写回过滤结果或统计输出，稳定 I/O 与资源回收边界。

这也是为什么需要 [`算法`](../algorithms/) 这一层：本页说明“为什么这些阶段边界必须存在”，算法页再说明“这些阶段今天具体做什么”。

## 控制界面

FastQTools 之所以保持可评审，是因为几条关键控制界面始终显式存在：

- CLI 参数与配置负责表达命令意图，但不会绕过分阶段核心；
- 公共头文件暴露受支持接口，而不会把任意内部所有权技巧直接泄露给调用者；
- benchmark 与 reference 章节独立于实现代码存在，但它们仍然描述同一套系统模型。

这些界面让读者可以从首页叙事平滑走到代码级细节，而不用重新学习另一套“内部架构”说法。

## 架构评审清单

评审未来变更时，可以用这张清单对照当前系统模型：

| 问题 | 可接受答案 |
| --- | --- |
| 是否保留 `source → processing → sink` 路径？ | 新行为进入明确阶段边界，而不是创建无人维护的旁路。 |
| 是否保留记录所有权清晰度？ | 任何 `std::string_view` 都仍然受所属 batch 生命周期约束。 |
| 是否保持公共 API 与实现细节分离？ | 受支持调用者仍然通过 `include/fqtools/` 或 CLI 表面进入。 |
| 是否保持证据与实现代码分离？ | benchmark 与发布口径仍然由文档、RFC 或 benchmark 报告承载，而不是只散落在源码注释里。 |

## 关键权衡

FastQTools 最关键的架构权衡之一，是把内存所有权当成一条一等边界来处理：

- `FastqBatch` 持有连续存储，减少逐条记录复制；
- `std::string_view` 让遍历保持轻量，但前提是批次生命周期绝不能被破坏；
- 对象池复用控制分配抖动，但前提是 in-flight 批次数量与复用时机始终受控。

这正是 <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md">RFC-0004</ReferenceBadge> 的重点。也因此，白皮书可以把“吞吐”和“内存有界”写在同一套叙事里，而不会沦为口号。

## 故障收敛

这套架构同时也是一套故障收敛策略：

- ingest 失败应该在 I/O 边界被显式翻译，而不是悄悄滑进 processing；
- processing 失败不应该破坏记录所有权或结果统计；
- sink 与 CLI 层应该承担面向用户的报告与最终清理边界。

这样，“快路径”和“正确性路径”就不会分裂成两套互不相干的系统故事。

## 验证梯度

仅有架构并不能证明系统可靠，但它能告诉你下一步该去哪里验证：

- 回到 [`白皮书`](../whitepaper/)：补足产品边界与评审语境；
- 继续看 [`算法`](../algorithms/)：检查过滤、统计、执行与内存行为如何落成维护中的实现策略；
- 再看 [`性能总览`](../performance/)：核对这些边界是否真的反映到 benchmark 证据里；
- 最后看 [`研究附录 / 演进说明`](../research/evolution-notes)：理解这些边界为什么被保留下来。

## 继续下钻

如果你想把这些结构直接和测量结果对应起来，下一步看 [`性能总览`](../performance/)；如果你想理解这些阶段今天具体执行哪些受支持行为，下一步看 [`算法`](../algorithms/)；如果你想直接进入实现与代码契约，再继续看 [`开发者架构设计`](../dev/architecture)、[`核心设计`](../dev/design) 与 [`IO 模块`](../api/io)。
