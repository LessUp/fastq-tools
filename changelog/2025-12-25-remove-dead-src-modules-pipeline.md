# 2025-12-25 清理残留目录：移除 src/modules/pipeline

## 背景
- 仓库已存在新的流水线实现目录 `src/pipeline/`。
- `src/modules/pipeline/` 当前仅残留一个 `CMakeLists.txt`，且未被 CMake 引入、也无源码文件，属于历史遗留结构。

## 变更
- 删除 `src/modules/pipeline/CMakeLists.txt`（该目录随之消失）。

## 影响
- 仅清理无引用的残留构建入口，不影响现有编译与运行行为。
- 后续流水线相关实现统一以 `src/pipeline/` 为准。
