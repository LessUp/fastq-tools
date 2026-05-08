# FASTQ execution runtime 深化设计

## 背景

FastQTools 当前在 `filter` 与 `stat` 两条命令路径里各自维护批处理遍历、运行时策略推导、对象池生命周期与串/并行执行编排。两处实现都围绕 `FastqReader`、`FastqBatch`、TBB pipeline 和局部聚合展开，导致理解与修改同一概念时需要来回跳转，测试也只能分别证明两份近似 orchestration。

这说明“批驱动 FASTQ 执行”已经是一个真实概念，但还没有成为一个深模块。

## 目标

- 把 `filter` 与 `stat` 共享的批驱动执行逻辑收拢到一个内部深模块。
- 保持现有 `ProcessingPipelineInterface` 与 `StatisticCalculatorInterface` 作为调用侧模块，不新增公开 API。
- 让大多数 orchestration 测试通过同一运行时接缝完成，并提供确定性的测试适配器。
- 不牺牲当前生产路径的高吞吐实现。

## 非目标

- 本次不为外部嵌入方暴露新的公共执行接口。
- 本次不把设计拆成多个新的 source / engine / sink 模块。
- 本次不改变 `filter` / `stat` 的 CLI 行为与输出格式。

## 选定方案

引入一个内部模块 **FASTQ execution runtime**，由它统一拥有：

- 运行时策略推导
- batch 获取与复用
- 串行 / 并行执行模式选择
- 遍历循环与时序控制
- 批次级部分结果的归并

`filter` 与 `stat` 不再直接拥有 TBB plumbing，而是向该模块提供各自的批次工作逻辑与结果归并逻辑。

## 模块形状

建议新增内部头/实现（路径可微调，但保持内部可见）：

- `src/processing/execution_runtime.h`
- `src/processing/execution_runtime.cpp`

建议保留并复用已有内部概念，而不是再次平铺到调用侧：

- `RuntimePolicy`
- `PipelineExecutionPlan`

它们可以被 `ExecutionRuntime` 吸收或由其统一调度，但调用侧不再自己推导或选择执行模式。

## 运行时接缝

`ExecutionRuntime` 作为唯一新模块，对调用侧暴露一个内部接缝，输入是“执行计划 + 批次工作 + 归并逻辑”，而不是多个分散的浅模块。

概念上可表达为：

```cpp
ExecutionRuntime::run(plan, batchWork, reduce, commit?)
```

其中：

- `plan`：输入路径、可选输出目标、`ProcessingOptions`、需要的执行统计上下文
- `batchWork`：对单个 `FastqBatch` 执行命令特定逻辑，返回部分结果
- `reduce`：把部分结果合并到最终结果
- `commit`：可选；当命令需要输出 batch 时执行，例如 `filter`

关键约束：

- 接缝保持**内部**，不进入 `include/fqtools/`
- 只服务当前真实调用方 `filter` 与 `stat`
- 不为未来假设场景提前引入更多外部 seam

## 适配器

本次只引入两个真实适配器：

### 1. 生产适配器

职责：

- 使用当前文件型 `FastqReader` / `FastqWriter`
- 保留对象池与并行执行能力
- 在运行时内部选择串行或并行路径

它保留现有高吞吐实现，但把 reader / writer 生命周期、batch 获取和执行编排集中到 `ExecutionRuntime` 里。

### 2. 确定性测试适配器

职责：

- 以预构造的 `FastqBatch` fixture 驱动同一运行时接缝
- 使用串行、内存内执行，避免线程调度和临时文件带来的偶然性
- 证明调用约定、批次顺序、归并与提交时机

它不是新的公共模块，只是运行时接缝上的测试适配器。

## 调用侧重构

### `ProcessingPipeline`

`ProcessingPipeline` 收缩为命令领域模块，只保留：

- predicate / mutator 组装后的批次处理逻辑
- `ProcessingStatistics` 的部分结果构造与归并
- `filter` 输出需要的 commit 行为

它不再直接拥有：

- `FastqReader` / `FastqWriter` 创建逻辑
- TBB pipeline 组装
- 运行模式选择

### `FastqStatisticCalculator`

`FastqStatisticCalculator` 收缩为统计领域模块，只保留：

- 单 batch 统计分析
- `FqStatisticResult` 归并
- 最终 report / sidecar 写出

它不再直接拥有：

- batch traversal
- 对象池获取
- TBB pipeline 组装

## 测试策略

### 新增测试

把测试面集中到 `ExecutionRuntime` 接缝：

- 串行 / 并行模式选择
- batch 顺序与遍历终止条件
- `reduce` 调用次数与归并顺序
- `commit` 只在需要输出的调用侧触发
- 生产适配器与确定性测试适配器遵守同一接缝契约

### 保留测试

- `filter` / `stat` 的命令级集成测试
- 真实 FASTQ 输入输出的 e2e 测试
- predicate / mutator / statistic worker 各自的行为测试

### 删除或收缩的测试

当前仅为证明两份重复 orchestration 而存在的测试，应在运行时接缝测试稳定后删除或改写，避免同一执行语义在 `processing_pipeline.cpp` 和 `fq_statistic.cpp` 中重复验证。

## 迁移顺序

1. 提取 `ExecutionRuntime` 骨架，并把现有 `RuntimePolicy` / `PipelineExecutionPlan` 接入该模块。
2. 先迁移 `ProcessingPipeline`，让 `filter` 走新运行时。
3. 再迁移 `FastqStatisticCalculator`，让 `stat` 走同一运行时。
4. 删除重复 orchestration 代码与被其绑定的冗余测试。

## 风险与约束

- **风险：** 把接缝设计得过于通用，重新制造一个浅抽象层。  
  **约束：** 只覆盖 `filter` / `stat` 当前共同需要的计划、工作、归并、提交四类能力。

- **风险：** 热路径抽象层引入额外开销。  
  **约束：** 保持接缝内部化，并优先使用可内联的轻量 callable 形状。

- **风险：** 调用侧仍残留运行时判断，导致 locality 不完整。  
  **约束：** 任何执行模式、batch 生命周期与对象池判断都必须回收到 `ExecutionRuntime`。

## 预期结果

完成后，FastQTools 会拥有一个真正负责任的 **FASTQ execution runtime** 模块。调用侧接口更小，运行时规则更集中，测试将围绕同一接缝展开，未来继续调整批处理执行策略时也只需要在一个地方改变实现。
