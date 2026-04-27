# Schema Spec Delta: Configuration & Data Models

## MODIFIED Requirements

### Requirement: memory policy schema 必须是可选且可观测的

**原定义**: [`openspec/baseline/schemas/schema.md`](../../../../baseline/schemas/schema.md)

**变更内容**:

后续若为 memory policy 增加 scratch allocator policy、pool reuse 开关、telemetry 输出或 memory-bound tuning 字段，这些 schema SHALL 以 additive、optional 的方式引入。现有 runtime 配置与 CLI 参数 SHALL 在未启用新字段时继续有效。

#### Scenario: 为 memory policy 扩展可选 schema

- **GIVEN** 后续实现需要新增 memory policy 相关配置
- **WHEN** schema 被扩展
- **THEN** 新字段 SHALL 以加法方式引入
- **AND** 新字段 SHALL 默认可选
- **AND** 现有 runtime 与 CLI 配置 SHALL 继续有效
