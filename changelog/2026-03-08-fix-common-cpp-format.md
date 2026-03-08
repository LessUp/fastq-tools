# fix(style): 修复 src/common/common.cpp 格式检查失败

- `Logger::log` 中 `fmt::format` 调用被不必要地换行，合并为单行以通过 clang-format 检查
