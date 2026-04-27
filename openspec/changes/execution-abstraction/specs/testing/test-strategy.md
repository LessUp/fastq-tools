# Testing Spec Delta: FastQTools

## MODIFIED Requirements

### Requirement: 执行抽象必须验证默认路径兼容与后端一致性

**原定义**: [`openspec/baseline/testing/test-strategy.md`](../../../../baseline/testing/test-strategy.md)

**变更内容**:

当 execution abstraction 进入实现阶段时，测试策略 SHALL 扩展为覆盖默认 oneTBB 路径兼容性、backend parity、lifecycle safety 与 execution configuration 开关行为。未启用新路径时，当前 `stat` / `filter` 行为 SHALL 继续通过现有测试。

#### Scenario: 引入 execution abstraction 后扩展测试矩阵

- **GIVEN** 后续实现引入 execution abstraction
- **WHEN** 新 backend 或 execution config 被加入
- **THEN** 测试集 SHALL 验证默认 oneTBB 路径兼容性
- **AND** 测试集 SHALL 验证 backend parity 或明确记录差异
- **AND** 在新路径禁用时，当前行为 SHALL 继续通过现有测试
