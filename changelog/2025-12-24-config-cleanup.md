# 2025-12-24 配置调整：对齐 MongoDB 风格

- `.clang-tidy`：移除 MongoDB `mongo-*` 自定义检查与选项，将 HeaderFilterRegex 调整为仅覆盖本仓 `src/`、`include/`、`tests/`。
- `.clang-format`：将 include 分类中的 `"mongo/.*"` 替换为 `"fqtools/.*"`，按本仓头文件前缀进行包含排序。
- `.editorconfig`：补充 Markdown 的行尾空格保留规则（避免破坏 Markdown 硬换行语义）。
- `.git-blame-ignore-revs`：移除外部仓库（MongoDB）的提交哈希列表，仅保留本仓自用说明。
- `docs/dev/coding_standards.md`：更新风格来源与格式化命令示例，明确“借鉴 MongoDB，但去掉 MongoDB 专用配置”。
