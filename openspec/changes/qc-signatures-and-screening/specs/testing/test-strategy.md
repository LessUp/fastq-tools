# Testing Spec Delta: FastQTools

## MODIFIED Requirements

### Requirement: signature 与 structured outputs 必须验证确定性和兼容性

**原定义**: [`openspec/baseline/testing/test-strategy.md`](../../../../baseline/testing/test-strategy.md)

**变更内容**:

当 qc signatures 或 structured outputs 进入实现阶段时，测试策略 SHALL 扩展为覆盖 deterministic signature behavior、duplicate metric consistency、structured report validation 与 disabled-path compatibility。未启用新路径时，当前文本统计与 CLI 行为 SHALL 继续通过现有测试。

#### Scenario: 引入 lightweight signature 后扩展测试矩阵

- **GIVEN** 后续实现引入 signature 或 structured output 能力
- **WHEN** 新能力进入实现
- **THEN** 测试集 SHALL 验证结果确定性与报告有效性
- **AND** 测试集 SHALL 验证 disabled-path compatibility
- **AND** 当前文本统计与 CLI 行为 SHALL 继续通过现有测试
