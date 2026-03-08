# CI 修复：安装 libc++ 开发包

- **日期**: 2025-03-08
- **类型**: fix
- **范围**: ci

## 变更内容

在 `.github/workflows/ci.yml` 的所有 Clang 21 安装步骤中添加 `libc++-21-dev` 和 `libc++abi-21-dev`。

## 原因

Conan profile (`config/conan/profile-clang`) 指定了 `compiler.libcxx=libc++`，
导致 Conan 构建依赖（如 GTest）时使用 `-stdlib=libc++` 链接。
CI runner 上未安装 libc++ 开发包，链接器报错：`cannot find -lc++: No such file or directory`。

## 影响的 Job

- `lint`
- `build-and-test`（clang matrix）
- `coverage`
