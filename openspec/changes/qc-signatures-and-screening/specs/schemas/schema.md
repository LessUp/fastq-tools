# Schema Spec Delta: Configuration & Data Models

## MODIFIED Requirements

### Requirement: signature 与 structured output schema 必须保持可选

**原定义**: [`openspec/baseline/schemas/schema.md`](../../../../baseline/schemas/schema.md)

**变更内容**:

后续若为 signature modules、duplicate metrics、contamination hints 或 structured side reports 增加 schema，这些字段和输出 SHALL 以 additive、optional 的方式引入。当前文本统计报告 SHALL 在未启用新路径时继续保持有效。

#### Scenario: 为 lightweight signature 与 side reports 扩展 schema

- **GIVEN** 后续实现需要新增 signature 或 structured output schema
- **WHEN** schema 被扩展
- **THEN** 新字段或新输出 SHALL 以加法方式引入
- **AND** 新能力 SHALL 默认可选
- **AND** 当前文本统计报告 SHALL 继续有效
