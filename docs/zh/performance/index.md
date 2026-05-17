# 性能总览

性能章节不只是展示一组数字，而是帮助你判断：这些结果来自哪些架构选择、在什么前提下成立、以及哪些边界会改变结论。

## 证据摘要

当前白皮书引用的代表性快照来自维护中的标准场景：**100K reads、150 bp、AMD Ryzen 9 5900X、Release 构建**。在这个口径下，读入路径约 **1696 MB/s**、写出路径约 **1.76M reads/s**、组合过滤约 **1.67M reads/s**、完整统计约 **302 MB/s**。这些数字的价值不是“证明永远这么快”，而是说明 FastQTools 已经落在一个对 FASTQ QC 有意义的吞吐量级，并且不同路径的量纲、瓶颈和使用语境是分开的。

证据并不只有一张结果表：本页负责解释这些数字为什么可信，[`Benchmark 报告`](./benchmark-report) 提供代表性结果摘要，[`Benchmark 指南`](../dev/benchmark-guide) 说明复现方法，[`RFC-0003`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md) 与 [`RFC-0006`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md) 则定义 benchmark 系统和维护边界。

<DiagramFrame
  asset="execution-model"
  caption="执行模型：性能证据需要和执行路径、方法学与维护边界一起理解。"
/>

## benchmark 如何解读

建议按 **叙事层 → 结果层 → 方法层 → 规范层** 的顺序阅读：先在本页确认“这些数字想回答什么问题”，再到 [`Benchmark 报告`](./benchmark-report) 看代表性结果，接着去 [`Benchmark 指南`](../dev/benchmark-guide) 检查采样方式、构建模式和脚本入口，最后用 RFC-0003 / RFC-0006 判断这些结果是一次性观察、维护中的快照，还是发布口径下的约束。

不要只盯着原始数字。单看 MB/s 或 reads/s，很容易把读取、过滤、统计这几条路径混成同一个结论，也容易忽略测试输入、线程设置和发布策略。对采用者来说，更重要的问题通常是：结果是否覆盖了和你相近的工作负载、方法是否可复现、架构是否能解释结果、项目是否定义了回归和更新策略。

## 风险边界

公开结果是**代表性样本**，不是普适常数。至少要注意这些边界条件：

- **压缩比例与编解码成本**：gzip 压缩级别、输入可压缩性不同，会直接改变读写阶段的 CPU 开销。
- **存储 I/O**：NVMe、网络盘、容器卷或共享文件系统的吞吐差异，可能让 benchmark 更像磁盘测试而不是解析测试。
- **线程数与并发参数**：单线程结果和多线程流水线结果不能直接类比；线程过多也可能引入争用、调度噪声和 NUMA 影响。
- **输入分布**：read 长度、质量分布、过滤谓词组合、通过率高低都会改变处理路径的热点。
- **机器拓扑**：CPU 微架构、缓存层级、内存带宽、SMT/超线程和容器限制，都会影响“同一套代码”最终跑出的曲线。

因此，页面里的数值更适合回答“是否值得继续评估”，而不是替你的生产环境做容量承诺。真正要做迁移或 SLA 判断时，应当回到项目提供的方法学与维护政策自行复现。

## 推荐阅读顺序

1. 先看本页理解证据框架
2. 再读 [`Benchmark 报告`](./benchmark-report)
3. 需要复现方法时再看 [`Benchmark 指南`](../dev/benchmark-guide)
4. 需要核对长期维护口径时，看 [`RFC-0003`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md) / [`RFC-0006`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md)

## 回到白皮书主线

如果你还没建立系统结构心智模型，先回到 [`架构`](../architecture/)；如果你已经准备行动，继续到 [`工作流`](../workflows/) 或 [`参考导航`](../reference/)。
