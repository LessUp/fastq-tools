# 性能

FastQTools 把性能文档放在叙事层，不是为了“营销数字”，而是为了帮助你判断：**这些数字意味着什么、在什么前提下成立、什么时候值得继续追下去。**

## 先看结论，再决定是否下钻

当前维护中的代表性结果来自 **100K reads / 150 bp / AMD Ryzen 9 5900X / Release 构建**：

- FASTQ 读取路径约 **1696 MB/s**
- FASTQ 写出路径约 **1.76M reads/s**
- 组合过滤路径约 **1.67M reads/s**
- 完整统计路径约 **302 MB/s**

这些数字首先回答的是：**FastQTools 是否处在你预期的 FASTQ QC 吞吐量级上。** 它们不是对所有机器、所有压缩比、所有参数组合的承诺，更不是跨工具“跑分竞赛”的全部答案。

进一步的数字与上下文，请看 [`英文 Benchmark 报告`](./benchmark-report)。

<DiagramFrame
  asset="execution-model"
  caption="执行模型：代表性性能数字来自 source → processing → sink 路径及其有界验证闭环。"
/>

## 这页应该如何使用

### 你在做采用评估时

你的问题通常不是“有没有 benchmark”，而是：

- benchmark 是否覆盖了与我相似的工作负载；
- 数字是否来自可复现的方法，而不是一次偶然跑分；
- 架构设计是否真的足以解释这些结果；
- 当结果变化时，项目有没有维护策略，而不是只保留过时截图。

因此，这一页把性能证据拆成四层：

| 证据层 | 作用 | 去哪里看 |
| --- | --- | --- |
| 叙事层 | 判断数字是否值得继续追问 | 当前页面 |
| 结果层 | 看代表性指标与结果摘要 | [`英文 Benchmark 报告`](./benchmark-report) |
| 方法层 | 看 benchmark 如何运行、如何产出报告 | [`Benchmark 指南`](../dev/benchmark-guide) |
| 规范层 | 看长期维护、阈值与 SLA 如何定义 | [`RFC-0003`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md)、[`RFC-0006`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md) |

## 为什么这些结果在架构上说得通

FastQTools 的性能主张并不是孤立数字，而是和它的实现路线绑定的：

- **零拷贝批处理模型** 减少了解析阶段的字符串复制；
- **oneTBB 并行流水线** 让处理阶段更容易吃满多核；
- **有界资源模型** 让高吞吐与可控内存占用可以同时讨论；
- **统一脚本入口与 benchmark 维护策略** 降低“本地快、CI 慢、文档过时”的解释成本。

如果你想先理解这些性能理由，再看 benchmark 数字，建议先去 [`架构`](../architecture/)。如果你反过来已经先被数字吸引，再回看架构，也完全合理。

## 不要过度解读的地方

FastQTools 的 benchmark 页面刻意强调“代表性结果”，就是为了避免几种常见误用：

1. **把单一硬件结果误当成所有环境 SLA**；
2. **把统计路径与过滤路径混成同一指标**；
3. **忽略存储、压缩、线程、数据分布对吞吐的影响**；
4. **把它当成跨工具营销表，而不是决策参考。**

如果你的问题是“我的环境能不能复现实测结果”，请直接进入：

- [`Benchmark 指南`](../dev/benchmark-guide)
- [`RFC-0006: Benchmark Maintenance and Release Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0006-benchmark-maintenance-policy.md)

## 典型阅读路径

### 场景 A：我要判断是否值得试用

[`为什么选择 FastQTools`](../why-fastqtools/) → [`性能总览`](./) → [`快速开始`](../guide/getting-started)

### 场景 B：我要验证某个吞吐主张

[`英文 Benchmark 报告`](./benchmark-report) → [`Benchmark 指南`](../dev/benchmark-guide) → [`RFC-0003: Benchmark System`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md)

### 场景 C：我要理解性能是否来自靠谱的架构

[`架构`](../architecture/) → [`开发者架构设计`](../dev/architecture) → [`核心设计`](../dev/design)

## 下一步

- 想先回到定位问题：[`为什么选择 FastQTools`](../why-fastqtools/)
- 想理解结构原因：[`架构`](../architecture/)
- 想找实操或接口文档：[`参考导航`](../reference/)
- 想看项目内外部延伸资料：[`资源中心`](../resources/)
