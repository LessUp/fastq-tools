# 性能总览

性能页的意义不只是展示数字，而是把 benchmark 当成一条“可维护的论证链”来阅读：哪些工作负载被代表、哪些前提被固定、以及结论在哪些边界下必须保持条件化。

## 证据摘要

当前维护中的代表性快照仍然来自 baseline 规定的标准场景：**100K reads、150 bp、AMD Ryzen 9 5900X、Release 构建**。在这个口径下，读入路径约 **1696 MB/s**、写出路径约 **1.76M reads/s**、组合过滤约 **1.67M reads/s**、完整统计约 **302 MB/s**。

重复这些数字的目的，不是承诺“所有数据集都会这样”，而是说明 FastQTools 已经进入对 FASTQ QC 有意义的吞吐量级，并且项目给出了足够的方法背景，使这些数字可以被复核、被讨论、被约束。

<DiagramFrame
  asset="execution-model"
  caption="执行模型：性能证据需要和执行路径、方法学与维护边界一起理解。"
/>

## benchmark 如何解读

建议把性能材料看成一条梯度，而不是一张单独的 benchmark 表；最稳妥的顺序仍然是 **叙事层 → 结果层 → 方法层 → 规范层**：

1. **叙事层** —— 先在本页理解这些数字到底要回答什么问题；
2. **结果层** —— 再去 [`Benchmark 报告`](./benchmark-report) 看代表性快照；
3. **方法层** —— 继续看 [`Benchmark 指南`](../dev/benchmark-guide) 理解复现方式；
4. **策略层** —— 最后用 <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md">RFC-0003</ReferenceBadge> 与 <ReferenceBadge kind="RFC" href="https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md">RFC-0006</ReferenceBadge> 判断这些结果是如何采集、如何保存、何时会影响发布口径。

如果跳过其中任何一层，通常就会把结论说得过满。因此，性能页最好与 [`架构`](../architecture/) 和 [`算法`](../algorithms/) 一起读。

## 风险边界

公开结果是**代表性样本**，不是普适常数。最重要的可变条件包括：

- **压缩比例与编解码成本**：gzip 压缩级别、输入可压缩性不同，会直接改变读写阶段的 CPU 开销。
- **存储 I/O**：NVMe、网络盘、容器卷或共享文件系统的吞吐差异，可能让 benchmark 更像磁盘测试而不是解析测试。
- **线程数与并发参数**：单线程结果和多线程流水线结果不能直接类比；线程过多也可能引入争用、调度噪声和 NUMA 影响。
- **输入分布**：read 长度、质量分布、过滤谓词组合、通过率高低都会改变处理路径的热点。
- **机器拓扑**：CPU 微架构、缓存层级、内存带宽、SMT/超线程和容器限制，都会影响“同一套代码”最终跑出的曲线。

因此，更好的问题通常不是“1696 MB/s 是不是绝对真理”，而是“项目是否给出了值得进一步评估的结果、方法和维护政策”。真正进入迁移或 SLA 判断时，必须自己复现实验，并回到政策文件核对口径。

## 不要只看 benchmark 报告

当原始指标已经读清楚后，再用研究层补足上下文，避免把数字孤立解读：

- [`研究附录 / 书目`](../research/bibliography) 收集了格式、架构与 benchmark 术语背后的正式来源；
- [`相关项目`](../research/related-projects) 帮助你把 FastQTools 与 FastQC、fastp、Cutadapt、seqtk 放回同一语境，而不是做“冠军榜”；
- [`演进说明`](../research/evolution-notes) 解释为什么 benchmark 政策与内存策略会被提升为长期维护的架构问题。

## 回到白皮书主线

如果你还没建立整体系统模型，先回到 [`白皮书`](../whitepaper/) 与 [`架构`](../architecture/)；如果你想理解被测量的行为本身，继续看 [`算法`](../algorithms/)；如果你已经准备查具体命令或接口，继续到 [`参考导航`](../reference/)。
