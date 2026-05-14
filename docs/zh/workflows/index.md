# 工作流

FastQTools 的工作流页不是命令速查表，而是把“**我现在要完成什么**”翻译成一条可执行阅读路径。新用户通常不缺文档链接，真正缺的是：面对一组深度资料，应该先读哪几页，才能最快完成验证、接入或贡献。

## 先选你的目标，而不是先选文档目录

| 你的当前目标 | 最短路径 | 适合谁 |
| --- | --- | --- |
| 先判断值不值得继续试 | [`为什么选择 FastQTools`](../why-fastqtools/) → [`性能总览`](../performance/) → [`快速开始`](../guide/getting-started) | 评估者、技术负责人 |
| 先把一个 FASTQ 文件跑通 | [`快速开始`](../guide/getting-started) → [`CLI 参考`](../guide/cli-reference) | 新用户、分析人员 |
| 把过滤/修剪放进现有 QC 流程 | 本页 → [`配置说明`](../guide/configuration) → [`部署指南`](../guide/deployment) | 平台工程、运维 |
| 把能力接进 C++ 应用 | 本页 → [`API 概览`](../api/overview) → [`架构`](../architecture/) | SDK/平台开发者 |
| 为项目补丁或维护 | 本页 → [`构建指南`](../dev/build) → [`测试策略`](../dev/testing) → [`参与贡献`](../contributing) | 贡献者、维护者 |

## 路径一：30 分钟内完成一次采用评估

如果你还不确定是否要真正投入时间，建议按下面顺序读：

1. [`为什么选择 FastQTools`](../why-fastqtools/)：确认它解决的是不是你的问题；
2. [`性能总览`](../performance/)：判断性能数字是否与你的工作负载同类；
3. [`快速开始`](../guide/getting-started)：确认安装与第一个命令成本是否可接受；
4. [`CLI 参考`](../guide/cli-reference)：检查常用参数是否覆盖你当前的 QC 习惯。

**适合什么时候停？**
如果到这一步你已经知道：命令模型合理、性能证据足够、部署成本可控，那就可以进入真正的试用；否则可以带着明确问题去看更深的架构或 benchmark 资料，而不是盲读全文档站。

## 路径二：把 `stat` / `filter` 接到现有质控流程里

这是最常见的实际采用路径。

### 你需要确认的不是只有命令语法

除了命令本身，你通常还要回答：

- 输入是 `.fastq` 还是 `.fastq.gz`，输出要不要压缩；
- 线程数、批大小、日志级别与环境约束怎么选；
- 统计结果、过滤输出和 sidecar 文件应如何纳入后续流程；
- 部署时如何在本地、CI、容器里保持一致。

### 建议阅读顺序

1. [`快速开始`](../guide/getting-started) —— 先跑通最小命令；
2. [`CLI 参考`](../guide/cli-reference) —— 再补齐参数细节；
3. [`配置说明`](../guide/configuration) —— 处理环境变量、配置与默认值；
4. [`部署指南`](../guide/deployment) —— 把命令放入容器或生产环境；
5. [`性能总览`](../performance/) —— 如果吞吐是关键指标，再回来看证据与前提。

## 路径三：从“终端工具”迁移到“可嵌入能力”

很多团队会先用 CLI 证明价值，再决定是否嵌入内部 C++ 系统。这个阶段不应该直接跳进源码，而应该先确认接口边界。

推荐顺序：

1. [`API 概览`](../api/overview) —— 看公开入口与模块划分；
2. [`架构`](../architecture/) —— 理解 CLI、公共 API、实现层之间的关系；
3. [`IO 模块`](../api/io) / [`处理模块`](../api/processing) / [`统计模块`](../api/statistics) —— 按你的集成重点下钻；
4. [`核心设计`](../dev/design) —— 需要理解内存与并发取舍时再读。

## 路径四：成为维护者或提交可信补丁

FastQTools 的维护路径强调“**先对齐 baseline，再动代码**”。因此你需要的不是立刻打开某个 `.cpp` 文件，而是先确认工具链、验证入口和设计边界。

推荐顺序：

1. [`构建指南`](../dev/build) —— 用仓库标准入口建立本地环境；
2. [`测试策略`](../dev/testing) —— 知道该跑哪些验证；
3. [`代码质量`](../dev/quality-tools) 与 [`编码规范`](../dev/coding-standards) —— 对齐提交质量；
4. [`开发者架构设计`](../dev/architecture) 与 [`核心设计`](../dev/design) —— 理解改动会落在哪一层；
5. [`参与贡献`](../contributing) —— 补齐协作方式；
6. 如需规范事实来源，再看 OpenSpec baseline：
   - [`RFC-0001: Core Architecture`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0001-core-architecture.md)
   - [`RFC-0002: Toolchain Policy`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0002-toolchain-policy.md)

## 路径五：性能验证与回归调查

当你已经能跑通命令，但要判断“慢在哪里、数字是否合理、回归是否真实”，建议不要只看单张 benchmark 表。

- 先看 [`性能总览`](../performance/)，理解对外数字的阅读方式；
- 再看 [`英文 Benchmark 报告`](../performance/benchmark-report)，获得代表性结果；
- 最后看 [`Benchmark 指南`](../dev/benchmark-guide) 与 [`RFC-0003: Benchmark System`](https://github.com/LessUp/fastq-tools/blob/master/openspec/baseline/architecture/0003-benchmark-system.md)，确认方法与维护策略。

## 如果你只想知道“下一步去哪”

- **我要先试命令**：[`快速开始`](../guide/getting-started)
- **我要找参数定义**：[`CLI 参考`](../guide/cli-reference)
- **我要理解为什么这样设计**：[`架构`](../architecture/)
- **我要找 API 和开发细节**：[`参考导航`](../reference/)
- **我要找外部背景资料与协作入口**：[`资源中心`](../resources/)
