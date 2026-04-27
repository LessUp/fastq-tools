# Testing Spec Delta: FastQTools

## MODIFIED Requirements

### Requirement: 高级 preprocessing 必须验证 pair-aware 路径与模块兼容性

**原定义**: [`openspec/baseline/testing/test-strategy.md`](../../../../baseline/testing/test-strategy.md)

**变更内容**:

当 advanced preprocessing 进入实现阶段时，测试策略 SHALL 扩展为覆盖 pair-aware 流程、mate consistency、orphan/singleton policy、adapter mismatch / overlap / wildcard/IUPAC 行为、UMI extract/tag/filter 与 preprocessing-only 边界、polyG/polyX 和 low-complexity 模块行为，以及新模块禁用时的兼容性保护。

#### Scenario: 引入高级 preprocessing 后扩展测试矩阵

- **GIVEN** 后续实现引入 advanced preprocessing 模块
- **WHEN** 这些模块进入实现
- **THEN** 测试集 SHALL 覆盖 pair-aware 与 metadata-aware 路径
- **AND** 测试集 SHALL 覆盖 orphan/singleton policy 与 adapter wildcard/IUPAC 策略
- **AND** 测试集 SHALL 验证 UMI 仍然停留在 preprocessing 范围内
- **AND** 测试集 SHALL 验证模块禁用时的兼容性
- **AND** 当前行为 SHALL 在默认路径下继续通过现有测试
