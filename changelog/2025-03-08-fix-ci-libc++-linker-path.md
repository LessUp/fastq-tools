# 修复 CI Clang 构建 libc++ 链接失败

- **日期**: 2025-03-08
- **类型**: fix
- **范围**: ci

## 问题

CI 环境（GitHub Actions）中 Conan 构建依赖（如 gtest）时，使用 Clang profile（`-stdlib=libc++`）
链接失败，报错 `/usr/bin/ld: cannot find -lc++: No such file or directory`。

## 原因

`libc++-21-dev` 安装的库文件位于 `/usr/lib/llvm-21/lib/`，不在链接器默认搜索路径中。

## 修复

在 `ci.yml` 的所有 LLVM 21 安装步骤后，将 `/usr/lib/llvm-21/lib` 导出到 `LIBRARY_PATH`
环境变量，确保链接器能找到 `libc++`。

涉及 job：`lint`、`build-and-test`（clang 矩阵）、`coverage`。
