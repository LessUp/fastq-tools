# Schema Spec Delta: Configuration & Data Models

## MODIFIED Requirements

### Requirement: advanced preprocessing schema 必须保持可选与组合式

**原定义**: [`openspec/baseline/schemas/schema.md`](../../../../baseline/schemas/schema.md)

**变更内容**:

后续若为 paired-end inputs、adapter/primer policy、UMI extraction、polyG/polyX 或 low-complexity 模块增加配置字段，这些 schema SHALL 以 additive、optional 的方式引入，并允许按模块组合使用。未启用新模块时，当前 CLI/runtime 配置 SHALL 继续有效。

#### Scenario: 为高级 preprocessing 模块扩展 schema

- **GIVEN** 后续实现需要新增 advanced preprocessing 配置
- **WHEN** schema 被扩展
- **THEN** 新字段 SHALL 以加法方式引入
- **AND** 新字段 SHALL 默认可选并可组合
- **AND** 当前 CLI/runtime 配置 SHALL 继续有效
