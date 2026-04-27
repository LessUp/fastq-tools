# API Spec Delta: Core Interfaces

## MODIFIED Requirements

### Requirement: 公共 API 可以暴露可选 memory policy 扩展点

**原定义**: [`openspec/baseline/api/core-api.md`](../../../../baseline/api/core-api.md)

**变更内容**:

FastQTools 的公共 API MAY 以加法方式引入 memory policy 相关扩展点，例如 scratch resource configuration、pool policy 选择或 allocation telemetry 查询接口，但这些扩展点 SHALL 保持可选，且 SHALL NOT 破坏现有 public API 的默认使用方式。

#### Scenario: 在公共边界内增加 memory policy 配置

- **GIVEN** 后续实现需要公开 memory policy 配置或观测接口
- **WHEN** 这些能力进入 public API
- **THEN** 它们 SHALL 保持可选扩展点
- **AND** 它们 SHALL NOT 破坏现有默认 public API 用法
- **AND** zero-copy ownership 规则 SHALL 继续有效
