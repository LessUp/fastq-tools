# API Spec Delta: Core Interfaces

## MODIFIED Requirements

### Requirement: 公共边界与未来扩展点

**原定义**: [`openspec/baseline/api/core-api.md`](../../../baseline/api/core-api.md)

**变更内容**:

FastQTools 的公共 API 边界 SHALL 继续限定在 `include/fqtools/`。在不直接扩展当前 baseline 接口集合的前提下，后续独立 proposal MAY 在该公共边界内为 execution abstraction、pair-aware processing、structured outputs 与 modular preprocessing 引入扩展点。

除非后续 proposal 明确修改生命周期约束，零拷贝 `FastqBatch`/`FastqRecord` 的 ownership 与 view 失效规则 SHALL 保持不变。

#### Scenario: 为后续能力预留 API 扩展空间

- **GIVEN** 当前 baseline 只承认 `include/fqtools/` 下已经存在的公共接口
- **WHEN** 后续 proposal 需要引入 execution abstraction、pair-aware processing、structured outputs 或 modular preprocessing
- **THEN** 扩展 SHALL 继续落在 `include/fqtools/` 公共边界内
- **AND** 当前零拷贝 ownership 与生命周期规则 SHALL 继续有效
- **AND** 任何改变 public boundary 或 zero-copy ownership 规则的行为 SHALL 需要后续 proposal 显式批准
