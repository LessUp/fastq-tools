# Testing Spec Delta: FastQTools

## MODIFIED Requirements

### Requirement: 后续实现的测试扩展与兼容性保护

**原定义**: [`openspec/baseline/testing/test-strategy.md`](../../../baseline/testing/test-strategy.md)

**变更内容**:

后续实现工作在引入 execution abstraction、memory policy、advanced preprocessing 或 structured outputs 时，测试策略 SHALL 同步扩展，覆盖 pair-aware 路径、execution/backend parity、SIMD 与 memory-policy 开关、structured outputs，以及相关文档验证。

当这些新路径被禁用或尚未启用时，现有 `stat`/`filter` 行为与当前测试通过条件 SHALL 继续保持不变。

#### Scenario: 为后续能力增加测试覆盖

- **GIVEN** 后续 proposal 逐步引入 pair-aware、backend abstraction、SIMD/memory policy 或 structured output 能力
- **WHEN** 这些能力进入实现阶段
- **THEN** 测试集 SHALL 增加对应路径与开关的覆盖
- **AND** 不同执行后端之间 SHALL 验证行为一致性或明确记录差异
- **AND** 在新路径禁用时，当前行为 SHALL 继续通过现有测试与文档验证
