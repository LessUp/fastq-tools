# 2025-12-24 代码风格：升级 Windsurf workflow 并统一格式化

## 变更概述
- 更新 `.windsurf/workflows/code-style-review.md`：将 `/code-style-review` 补强为可执行的端到端流程（安全前置、范围控制、格式化→复检→静态分析→测试→记录）。
- 修正 `scripts/lint.sh` 源码扫描范围：覆盖 `src/`、`include/`、`tests/`，移除不存在的 `app/`，确保全局格式化覆盖公共头文件。
- 调整顶层 CMake：`BUILD_BENCHMARKS` 默认改为 `OFF`，避免默认构建因缺少 Google Benchmark 依赖而失败，保证可生成 `compile_commands.json` 供 clang-tidy 使用。

## 后续操作建议
- 在可写分支运行：
  - `./scripts/build.sh clang Release`
  - `./scripts/lint.sh format`
  - `./scripts/lint.sh format-check`
  - `./scripts/lint.sh lint --build-dir build-clang-release`
  - `./scripts/test.sh`

## 回退方案
- 如需回退：撤销上述文件变更或回滚对应提交。
