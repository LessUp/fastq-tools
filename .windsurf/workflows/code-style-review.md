---
title: "/code-style-review"
description: "在 FastQTools 仓库执行一致、可维护的编码风格检查与修复流程。"
auto_execution_mode: 1
---

## 使用方式
- 在 FastQTools 仓库根目录执行 `/code-style-review`
- 仅在可写分支运行；若遇只读限制，请提示用户自行执行命令
- 默认以“不破坏第三方/生成文件”为前提，必要时先征求确认
- 先确认工具可用：`clang-format`、`clang-tidy` 在 PATH 内，`scripts/lint.sh` 具备执行权限

## 工作流步骤
1. **初始化与安全前置**
   // turbo
   - `git status -sb` 检查脏区；如有未提交修改，先询问是否继续（建议先提交/暂存/新建分支）
   // turbo
   - 记录当前分支：`git rev-parse --abbrev-ref HEAD`
   // turbo
   - 记录当前提交：`git rev-parse HEAD`
   // turbo
   - 工具预检：`command -v clang-format`、`command -v clang-tidy`；确认 `scripts/lint.sh` 可执行：`test -x scripts/lint.sh && echo OK`
   - 可选：创建工作分支（避免污染主分支）
     - `git checkout -b chore/style-unify-YYYYMMDD`

2. **基线规范确认**
   - 阅读 `docs/dev/coding_standards.md`，明确命名（代码/文档/脚本 snake_case）、成员前缀 `m_`、包含顺序等要点
   - 查看 `.clang-format`、`.clang-tidy`，确保与脚本配置一致
   - 用要点列表与用户快速对齐，如有补充要求先记录

3. **范围与排除策略（重要）**
   - 默认仅对本仓源码目录执行（避免误改第三方/生成文件）：
     - `src/`
     - `include/`
     - `tests/`
   - 明确不在本流程中格式化的目录（除非用户明确要求）：
     - `third_party/`、`dist/`、`build-*/`、`cmake/`（如需可单独加步骤处理）
   - 如需“仅格式化本次改动文件”，先生成文件清单：
     - 优先：`git diff --name-only origin/HEAD...`
     - 回退：`git diff --name-only HEAD`
     - 仅保留 C/C++ 文件（示例）：`... | grep -E '\\.(c|cc|cpp|cxx|h|hh|hpp|hxx|ipp|inl)$'`

4. **自动化：先格式化，再静态分析**
   - 如无 `build-clang-release/compile_commands.json`，先运行 `./scripts/build.sh clang Release`
   - 快速格式核查（无写操作）：`./scripts/lint.sh format-check`
   - 全局格式化（会改文件）：`./scripts/lint.sh format`
   - 格式复检（无写操作）：`./scripts/lint.sh format-check`
   - clang-tidy（会读 compile_commands）：`./scripts/lint.sh lint --build-dir build-clang-release`
   - 如需自动修复（会改文件，且可能改动较大），再执行：`./scripts/lint.sh lint-fix --build-dir build-clang-release`
   - 捕获输出；失败时保留末 100 行日志，便于溯源

5. **命名与结构巡检（可选辅助命令）**
   - 文档/脚本命名：`find docs scripts -type f ! -regex '.*/[a-z0-9][a-z0-9_]*\\.[a-z0-9]*' -print`
   - 源码命名：确认新文件遵循 snake_case（含目录），类为 PascalCase，常量为 UPPER_SNAKE_CASE，成员变量 `m_`
   - 目录：公共 API 仅放 `include/`，实现细节放 `src/`，避免交叉依赖

6. **C/C++ 代码核查**
   - `#include` 顺序：标准库 → 第三方 → 本项目
   - 头文件最小化依赖，必要时用前置声明；实现放源文件
   - 现代 C++：智能指针/`std::string_view`/尾随返回类型，禁止裸 `new/delete`

7. **验证**
   - 建议运行测试：`./scripts/test.sh`

8. **逐项修复与验证**
   - 对每个问题给出：位置、违反规则、修复方案；涉及第三方/生成文件先获准
   - 使用 `./scripts/lint.sh lint-fix` 或 `clang-format` 定点修复后重跑相关脚本

9. **总结与记录**
   - 汇总表：文件 | 问题 | 处理方式 | 脚本状态
   - 未完成项明确责任人和后续动作
   - 记录格式化提交：将“仅机械改动”的提交写入 `changelog/`，必要时可把该提交 hash 添加到 `.git-blame-ignore-revs`
