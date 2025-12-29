# 2025-12-25 error 模块命名重构（MongoDB 风格）

- `src/modules/error/error.{h,cpp}`：将成员变量从 `m_` 前缀改为 `camelCase_` 后缀：
  - `m_category` -> `category_`
  - `m_severity` -> `severity_`
  - `m_message` -> `message_`
  - `m_what_message` -> `whatMessage_`
  - `m_mutex` -> `mutex_`
  - `m_handlers` -> `handlers_`

