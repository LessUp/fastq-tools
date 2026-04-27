# Product Spec Delta: FASTQ Processing

## MODIFIED Requirements

### Requirement: 性能目标应包含资源可预测性与可观测性

**原定义**: [`openspec/baseline/product/fastq-processing.md`](../../../../baseline/product/fastq-processing.md)

**变更内容**:

FastQTools 的性能演进目标 SHALL 不仅关注吞吐量，也包括资源占用的可预测性与可观测性。后续 memory policy 可以引入 PMR、worker-local pools 与 allocation telemetry，但默认维护路径 SHALL 继续保持当前命令与输出兼容。

#### Scenario: 在性能目标中加入资源策略约束

- **GIVEN** 项目继续向高性能 preprocessing engine 演进
- **WHEN** memory policy 被纳入规范
- **THEN** 产品目标 SHALL 同时考虑吞吐与资源可预测性
- **AND** telemetry 或 allocator 策略 SHALL 不强制改变默认用户路径
- **AND** 当前 `stat` / `filter` 行为 SHALL 继续兼容
