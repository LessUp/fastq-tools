# 根目录配置文件检查与修复

**日期**: 2026-03-08
**类型**: fix(docs), chore

## 变更内容

### 修复 README.md
- 构建脚本路径 `./scripts/build.sh` → `./scripts/core/build`
- 构建产物路径 `./build-clang-release/` → `./build/clang-release/`
- 依赖清单移除项目未使用的 `bzip2 / xz`，替换为实际使用的 `zlib-ng / libdeflate`
- 编译器推荐版本从 `Clang 19` 更新为 `GCC 15 / Clang 21`
- 构建示例更新为当前 `scripts/core/build` 的参数格式

### 修复 CONTRIBUTING.md
- 脚本路径 `./scripts/format.sh`/`lint.sh`/`test.sh` → `./scripts/core/lint format`/`lint check`/`test`

### 修复 Doxyfile
- INPUT 添加 `include` 目录，确保公共 API 头文件被 Doxygen 文档化

### 修复 conanfile.py
- `package_info.libs` 移除不存在的 `fq_lib`（INTERFACE target），列出实际静态库目标
