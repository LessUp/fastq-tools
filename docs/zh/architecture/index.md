# 架构

本页不是把所有内部模块再抄一遍，而是给采用者一个 **“先建立心智模型，再决定下钻哪里”** 的入口。你应该在这里回答：FastQTools 的性能和可维护性，究竟来自哪些明确的架构选择？

## 一句话心智模型

FastQTools 采用的是 **CLI / 命令层 / 公共库接口 / 具体实现** 的分层结构，用 **零拷贝批处理模型** 承载 FASTQ 记录，用 **oneTBB 并行流水线** 组织吞吐路径，再用 **OpenSpec baseline + ADR** 约束长期维护中的设计漂移。

这意味着它既不是“只有命令没有内核”的一次性脚本，也不是“只有库没有操作入口”的纯 SDK，而是把用户入口和工程内核收敛到同一套设计原则里。

<DiagramFrame caption="架构总览：上层入口映射到同一套公共 API 与有界 source → processing → sink 执行路径。">
  <img src="../../assets/diagrams/architecture-overview.svg" alt="FastQTools 架构总览图，展示 CLI、命令层、公共 API、实现模块以及有界处理流水线。" />
</DiagramFrame>

## 从上到下看：谁负责什么

| 层次 | 你应该如何理解它 | 深入阅读 |
| --- | --- | --- |
| CLI 层 | 负责参数解析、命令分发、日志边界，是终端用户进入系统的第一层 | [`CLI 参考`](../guide/cli-reference)、[`开发者架构设计`](../dev/architecture) |
| 命令层 | 把 `stat`、`filter` 这类任务翻译成具体执行路径，承担参数校验与编排职责 | [`快速开始`](../guide/getting-started)、[`开发者架构设计`](../dev/architecture) |
| 公共库层 | 通过 `include/fqtools/` 暴露稳定接口，让 CLI 与外部集成都依赖同一套 API | [`API 概览`](../api/overview) |
| 实现层 | 真正完成 I/O、处理、统计、错误与配置管理，是性能与正确性的落点 | [`核心设计`](../dev/design)、[`IO 模块`](../api/io)、[`处理模块`](../api/processing) |

如果你只关心“能不能用”，上面的表已经够了；如果你关心“为什么它会这样表现”，下一部分更关键。

## 从左到右看：数据如何穿过系统

### 1. FASTQ 不是按“行字符串集合”来想，而是按“批次缓冲区 + 记录视图”来想

FastQTools 的核心数据移动方式，是让 `FastqBatch` 持有连续内存，让 `FastqRecord` 通过 `std::string_view` 指向那块批次内存。它带来的好处不是抽象上的优雅，而是非常实际的：

- 解析时减少字符串复制；
- 批处理更容易和并行流水线结合；
- 热路径的内存行为更容易解释和约束。

如果你需要确认这里是否只是文档口号，直接看：

- [`开发者架构设计`](../dev/architecture)
- [`核心设计`](../dev/design)
- [`IO 模块`](../api/io)
- [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)

### 2. 并行不是“ everywhere ”，而是围绕 source / processing / sink 组织

项目将读取与写回保持为顺序边界，把真正适合并行的工作留在处理中段。这种 `source → processing → sink` 的组织方式意味着：

- I/O 顺序更容易保证；
- 并行度主要投入到过滤、修剪、统计等计算环节；
- 背压与 in-flight 资源约束更容易被描述和测试。

这也是为什么架构文档里反复强调 **bounded resources** 与 **deterministic output**：性能不是唯一目标，可解释的边界同样重要。

### 3. 公共 API 与实现分离，保证 CLI 与嵌入式使用共享同一内核

采用者常见的担心是：命令行工具很好用，但一旦我要把它纳入 C++ 代码库，是否要重学另一套内部接口？FastQTools 的回答是尽量避免这种分裂：

- CLI 依赖公共 API，而不是直接绑定内部实现；
- `fq.h` 聚合作为外部入口；
- 具体模块继续细分到 `io`、`processing`、`statistics`、`config`、`error` 等命名空间。

如果你的采用场景包含“先 CLI 验证、再库集成”，这点尤其值得重视。

## 架构主张如何落到“可信”二字上

FastQTools 的文档体系不是只讲模块名，还把关键正确性属性写进 baseline：

- **内存安全**：不应因 FASTQ 解析或批次复用导致非法访问；
- **线程安全**：并行流水线不应引入数据竞争；
- **输出可重复理解**：同输入与同配置下，结果应保持可解释的一致性；
- **资源有界**：处理超大文件时内存占用不应无限增长。

这些主张的意义在于：你可以把它们和测试、sanitizer、benchmark 一起读，而不是把“架构”和“验证”拆成两套互不相干的材料。

## 推荐的下钻路径

### 如果你是采用评估者

1. 先读 [`为什么选择 FastQTools`](../why-fastqtools/)，明确它解决的问题；
2. 再读本页，建立分层与数据流心智模型；
3. 然后去 [`性能总览`](../performance/)，确认这些设计是否真的映射到证据上。

### 如果你是集成者

1. 从 [`API 概览`](../api/overview) 进入公共接口；
2. 需要理解批处理与记录生命周期时，再读 [`IO 模块`](../api/io)；
3. 需要理解过滤/修剪路径时，再读 [`处理模块`](../api/processing)。

### 如果你是维护者或贡献者

1. 先读 [`开发者架构设计`](../dev/architecture) 与 [`核心设计`](../dev/design)；
2. 再对照 OpenSpec baseline/ADR：
   - [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
   - [`RFC-0002: Toolchain Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)
   - [`RFC-0004: Memory Pool Optimization`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0004-memory-pool.md)

## 不必在这里读完所有细节

这页的目标是帮你判断：

- 项目的性能故事是不是建立在具体结构上；
- CLI、库接口和内部实现是否有清晰边界；
- 哪份深度文档最值得你花时间继续读。

如果答案是“是”，就继续前往：

- [`工作流`](../workflows/)：按角色看使用路径
- [`性能总览`](../performance/)：按证据看性能主张
- [`参考导航`](../reference/)：按任务找具体手册
