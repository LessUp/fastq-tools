---
title: "/static-analysis"
description: "运行 FastQTools 静态分析与代码质量检查（clang-format/clang-tidy/cppcheck/iwyu）。"
auto_execution_mode: 1
---

## 适用场景
- 你想在提交前做一次完整的代码质量检查
- CI 报告静态检查失败，需要本地复现

## 工作流步骤
1. **安全前置**
   // turbo
   - `git status -sb`

2. **准备构建目录（供 clang-tidy/iwyu 使用）**
   - 若无 `build-clang-release/compile_commands.json`：
     - `./scripts/core/build -c clang -t Release`

3. **clang-format（不写文件）**
   - `./scripts/core/lint format-check`

4. **clang-tidy（读取编译数据库）**
   - `./scripts/core/lint tidy -b build-clang-release`

5. **cppcheck（可选，但建议）**
   - `./scripts/core/lint cppcheck`

6. **IWYU（可选，建议人工审阅输出）**
   - `./scripts/core/lint iwyu -b build-clang-release`

## 备注
- 如需自动修复 clang-tidy：`./scripts/core/lint tidy-fix -b build-clang-release`（改动可能较大，建议单独提交）
