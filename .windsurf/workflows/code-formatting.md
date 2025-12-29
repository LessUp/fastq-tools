---
title: "/code-formatting"
description: "对 FastQTools 进行 clang-format/clang-tidy 代码质量检查与自动修复（复用 scripts/lint.sh）。"
auto_execution_mode: 1
---

## 适用场景
- 你修改了 C/C++ 代码，想要在提交前快速保证格式与静态检查过关
- 你希望先只检查（不改文件），确认没有格式问题

## 工作流步骤
1. **安全前置**
   // turbo
   - `git status -sb`
   - 若存在未提交修改：继续前先确认你是否希望在当前分支直接改动（`format`/`lint-fix` 会改文件）

2. **工具与脚本预检**
   // turbo
   - `command -v clang-format`
   // turbo
   - `command -v clang-tidy`
   // turbo
   - `test -x scripts/lint.sh && echo OK`

3. **仅检查（不写文件）**
   - 运行格式检查：`./scripts/lint.sh format-check`
   - 运行静态检查：`./scripts/lint.sh lint --build-dir build-clang-release`
     - 若提示缺少 `build-clang-release/compile_commands.json`，先运行：`./scripts/build.sh clang Release`

4. **自动修复（会写文件）**
   - 格式化：`./scripts/lint.sh format`
   - 可选：clang-tidy 自动修复（变更可能较大，建议单独提交）：`./scripts/lint.sh lint-fix --build-dir build-clang-release`

5. **复检与收尾**
   - `./scripts/lint.sh format-check`
   - `./scripts/lint.sh lint --build-dir build-clang-release`
   - 汇总哪些文件发生了修改：`git diff --name-only`
