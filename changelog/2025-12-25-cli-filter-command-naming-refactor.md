# 2025-12-25 CLI FilterCommand 命名重构（MongoDB 风格）

- `src/cli/commands/filter_command.h`：成员变量从 `m_` 前缀改为 `camelCase_` 后缀：
  - `m_config` -> `config_`
  - `m_pipeline` -> `pipeline_`
- `src/cli/commands/filter_command.cpp`：同步实现侧成员访问，并将内部 `Config` 字段改为 `camelCase`。

