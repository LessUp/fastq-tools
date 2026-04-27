# Testing Spec Delta: FastQTools

## MODIFIED Requirements

### Requirement: memory policy 必须验证资源回归、benchmark 与 sanitizer

**原定义**: [`openspec/baseline/testing/test-strategy.md`](../../../../baseline/testing/test-strategy.md)

**变更内容**:

当 memory resource policy 进入实现阶段时，测试策略 SHALL 扩展为覆盖 memory-bound regression、allocator/pool parity、telemetry 开关行为，以及 sanitizer 与 benchmark 验证。未启用新路径时，当前 `stat` / `filter` 行为 SHALL 继续通过现有测试。

#### Scenario: 引入 memory policy 后扩展验证矩阵

- **GIVEN** 后续实现引入 memory policy
- **WHEN** allocator、pool 或 telemetry 能力进入实现
- **THEN** 测试集 SHALL 验证资源回归与 parity
- **AND** benchmark 与 sanitizer SHALL 成为验收的一部分
- **AND** 在新路径禁用时，当前行为 SHALL 继续通过现有测试
