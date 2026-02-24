---
title: "/review-diff"
description: "提交前代码审查：分析 diff、检查潜在问题、生成审查清单。"
auto_execution_mode: 1
---

## 适用场景
- 提交前自我审查
- PR 提交前的最终检查
- 帮助发现容易遗漏的问题

## 工作流步骤
1. **查看改动范围**
   // turbo
   - `git diff --stat`（未暂存）
   - `git diff --cached --stat`（已暂存）
   - `git log --oneline -5`（最近提交历史）

2. **逐文件审查 diff**
   - `git diff`（或 `git diff --cached`）
   - 对每个改动文件，检查以下清单：

3. **审查清单**
   - [ ] **正确性**：逻辑是否正确？边界条件是否处理？
   - [ ] **命名**：是否遵循项目命名规范（PascalCase 类、camelCase 函数/变量、`_` 后缀私有成员）？
   - [ ] **错误处理**：异常是否正确捕获和传播？`ErrorContext` 是否完整？
   - [ ] **内存安全**：`string_view` 生命周期是否安全？`unique_ptr`/`shared_ptr` 使用是否合理？
   - [ ] **并发安全**：TBB pipeline 中的共享状态是否正确保护？
   - [ ] **性能**：是否有不必要的拷贝？是否可用 `std::move`？
   - [ ] **测试**：改动是否有对应测试覆盖？
   - [ ] **文档**：公共 API 变更是否更新了 Doxygen 注释？
   - [ ] **日志**：关键路径是否有适当的日志输出？
   - [ ] **头文件**：`#include` 是否最小化？是否遵循公共/内部路径规范？

4. **自动化检查**
   - `./scripts/core/lint format-check`
   - `./scripts/core/lint tidy -b build-clang-release`（可选）

5. **生成审查报告**
   - 汇总发现的问题
   - 按严重程度分类：🔴 必须修复 / 🟡 建议改进 / 🟢 无问题

## 备注
- 重点关注 `include/fqtools/` 下的公共 API 变更——这些是稳定接口
- `src/` 下的实现变更相对灵活，但注意与接口的一致性
- 大的重构建议拆分成多个小 PR
