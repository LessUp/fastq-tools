# API Spec Delta: Core Interfaces

## MODIFIED Requirements

### Requirement: 公共 API 可以暴露 signature calculators 与 structured output 扩展点

**原定义**: [`openspec/baseline/api/core-api.md`](../../../../baseline/api/core-api.md)

**变更内容**:

FastQTools 的公共 API MAY 以加法方式引入 signature calculator、duplicate estimator、contamination hint provider 与 structured report emitter 等扩展点，但这些扩展点 SHALL 保持 optional，并 SHALL NOT 被描述为外部服务化契约。

#### Scenario: 在公共边界内加入 lightweight signature 扩展点

- **GIVEN** 后续实现需要公开 signature 或 structured report 能力
- **WHEN** 新扩展点进入 public API
- **THEN** 它们 MAY 以加法方式加入公共头文件
- **AND** 它们 SHALL 保持 optional
- **AND** 它们 SHALL NOT 被定义为外部服务化契约
