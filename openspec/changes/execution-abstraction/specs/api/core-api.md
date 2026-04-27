# API Spec Delta: Core Interfaces

## MODIFIED Requirements

### Requirement: 公共 API 可以暴露执行抽象扩展点

**原定义**: [`openspec/baseline/api/core-api.md`](../../../../baseline/api/core-api.md)

**变更内容**:

FastQTools 的公共 API 边界 SHALL 继续限定在 `include/fqtools/`。若 execution abstraction 需要公开配置、工厂或 backend descriptor 等扩展点，这些能力 MAY 以加法方式出现在公共头文件内，但 SHALL NOT 暴露 `src/` 内部实现类型。

默认 public API 行为 SHALL 继续兼容当前 oneTBB-default 路径，除非用户显式选择新的可选 backend/configuration。

#### Scenario: 在公共边界内引入执行抽象扩展点

- **GIVEN** 后续实现需要公开 execution backend 相关扩展点
- **WHEN** 这些扩展点进入 public API 设计
- **THEN** 它们 SHALL 继续位于 `include/fqtools/` 边界内
- **AND** 它们 SHALL NOT 公开 `src/` 内部类型
- **AND** 默认行为 SHALL 继续兼容现有 oneTBB 路径
