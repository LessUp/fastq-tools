---
title: "/commit-and-push"
description: "格式化 → lint → 测试 → commit → push 一条龙流程，确保提交质量。"
auto_execution_mode: 1
---

## 适用场景
- 完成一组改动后，需要格式化、检查、测试、提交一步到位
- 确保 CI 不会因格式或 lint 问题失败

## 工作流步骤
1. **查看当前改动**
   // turbo
   - `git status -sb`
   - `git diff --stat`

2. **代码格式化**
   - `./scripts/core/lint format`（自动修复格式）
   - 如果有修改，查看 diff 确认无误

3. **静态检查**
   - `./scripts/core/lint format-check`（确认格式通过）
   - `./scripts/core/lint tidy -b build-clang-release`（可选，clang-tidy 检查）

4. **运行测试**
   - `./scripts/core/test -c clang -t Debug`
   - 如有失败，先修复再继续

5. **Stage 改动**
   - `git add -A`（或选择性 `git add <files>`）
   - `git diff --cached --stat`（确认暂存内容）

6. **提交**
   - 使用 Conventional Commits 格式：`<type>(<scope>): <subject>`
   - 示例：`git commit -m "feat(processing): 添加质量修剪器"`
   - type: feat | fix | docs | style | refactor | perf | test | build | ci | chore

7. **推送**
   - `git push origin HEAD`
   - 如果是新分支：`git push -u origin HEAD`

## 备注
- 如果 lint 或测试失败，先修复再提交，不要跳过
- commit message 使用中文描述，type/scope 用英文
