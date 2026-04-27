# Schema Spec Delta: Configuration & Data Models

## MODIFIED Requirements

### Requirement: Schema 演进必须保持加法兼容

**原定义**: [`openspec/baseline/schemas/schema.md`](../../../../baseline/schemas/schema.md)

**变更内容**:

后续针对 pair-aware processing、structured outputs 与 module configuration 的 schema 演进 SHALL 采用 additive 方式，并默认保持可选。除非后续 proposal 明确修改 baseline，当前 `stat` 文本报告行为与既有 CLI/runtime schema SHALL 继续保持有效。

#### Scenario: 扩展未来 schema 而不破坏当前行为

- **GIVEN** 当前 baseline 只定义了现有 CLI 参数、运行时结构体与 `stat` 文本报告
- **WHEN** 后续 proposal 为 pair-aware processing、structured outputs 或模块化 preprocessing 引入新 schema
- **THEN** 新字段或新配置 SHALL 以加法方式引入
- **AND** 新能力 SHALL 默认是可选的
- **AND** 当前文本报告行为 SHALL 在未批准基线变更前继续保留
