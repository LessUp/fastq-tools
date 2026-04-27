# Schema Spec Delta: Configuration & Data Models

## MODIFIED Requirements

### Requirement: 执行后端配置必须保持加法兼容

**原定义**: [`openspec/baseline/schemas/schema.md`](../../../../baseline/schemas/schema.md)

**变更内容**:

后续若为 execution abstraction 引入 backend identifier、stage tuning 或 execution telemetry 开关，这些 schema 变更 SHALL 采用 additive、optional 的方式落地。当前 `threadCount`、`batchSize` 与既有 CLI/runtime 配置 SHALL 继续保持有效。

#### Scenario: 为 backend 选择预留可选 schema

- **GIVEN** 后续实现需要为 execution backend 或调优开关增加配置
- **WHEN** schema 被扩展
- **THEN** 新字段 SHALL 以加法方式引入
- **AND** 新字段 SHALL 默认是可选的
- **AND** 现有 CLI/runtime 配置 SHALL 继续有效
