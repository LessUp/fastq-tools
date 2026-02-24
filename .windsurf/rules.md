---
description: FastQTools project rules for Cascade/Windsurf
---

# FastQTools: 项目规则（Windsurf/Cascade）

## 权威入口（优先使用）
- **构建**: `./scripts/core/build`
- **测试**: `./scripts/core/test`
- **代码质量**: `./scripts/core/lint`
- **依赖安装**: `./scripts/core/install-deps`
- **Benchmark 系统**: `./scripts/tools/benchmark`（基于 `tools/benchmark` + `benchmark_results/`）

## 禁止/避免
- 旧的 `.sh` 脚本已删除，所有功能已迁移至 `scripts/core/` 和 `scripts/tools/`。

## 构建系统约定
- **CMake + Ninja**（`CMakePresets.json` 也可参考，但常用入口仍是 `scripts/core/build`）。
- 默认构建目录命名：`build-<compiler>-<type>`，例如：
  - `build-clang-debug`
  - `build-clang-release`
  - `build-clang-coverage`
- Conan：`scripts/core/build` 会在可用时自动 `conan install config/dependencies/ ...` 生成 toolchain。

## 代码风格（权威文档）
- 以 `docs/dev/coding-standards.md` 为准。
- C++20；clang-format/clang-tidy 配置来自 `.clang-format` / `.clang-tidy`。
- 文件/目录：`snake_case`；文档：`kebab-case`；类：`PascalCase`；函数/变量：`camelCase`。
- include 顺序：标准库 -> 第三方 -> 本项目。

## 目录约定
- 公共 API：`include/fqtools/`
- 实现：`src/`
- CLI：`src/cli/`
- 测试：`tests/`
- Benchmark：`tools/benchmark/` + `benchmark_results/`

## 常用质量检查命令（建议优先级）
- `./scripts/core/lint format-check`
- `./scripts/core/lint tidy -b build-clang-release`
- `./scripts/core/lint cppcheck`（可选）
- `./scripts/core/lint iwyu -b build-clang-release`（可选，建议人工审阅输出）

## 分析/诊断工具
- Valgrind memcheck: `./scripts/tools/valgrind-memcheck`
- Valgrind cachegrind: `./scripts/tools/valgrind-cachegrind`
- Sanitizers: `./scripts/core/build --sanitizer asan|tsan|ubsan`（建议 Debug，且常搭配 `--no-lto`）
