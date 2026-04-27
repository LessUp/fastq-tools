# API Spec Delta: Core Interfaces

## MODIFIED Requirements

### Requirement: 公共 API 可以暴露 pair-aware 与 metadata-aware 扩展点

**原定义**: [`openspec/baseline/api/core-api.md`](../../../../baseline/api/core-api.md)

**变更内容**:

FastQTools 的公共 API MAY 以加法方式引入 pair-aware processing（含 orphan/singleton policy）、adapter/primer configuration（含 mismatch / overlap / wildcard/IUPAC 策略）、UMI extract/tag/filter 与 metadata propagation，以及 low-complexity/polyG policy 等扩展点，但这些扩展点 SHALL 保持与现有单端路径兼容，且 SHALL NOT 破坏 `FastqBatch` / `FastqRecord` 的所有权与生命周期规则。

UMI 相关扩展点 SHALL 被建模为 preprocessing 能力，而 SHALL NOT 强制绑定 dedup workflow。

#### Scenario: 在公共边界内加入高级 preprocessing 扩展点

- **GIVEN** 后续实现需要公开高级 preprocessing 配置或接口
- **WHEN** 新扩展点进入 public API
- **THEN** 它们 MAY 以加法方式加入公共头文件
- **AND** 单端路径 SHALL 继续兼容
- **AND** orphan/singleton policy 与 adapter wildcard/IUPAC 策略 SHALL 被显式建模
- **AND** 所有权与生命周期规则 SHALL 保持不变
