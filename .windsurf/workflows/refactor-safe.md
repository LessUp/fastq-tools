---
title: "/refactor-safe"
description: "安全重构：重命名/提取/移动 + 自动化验证全流程，确保重构不引入回归。"
auto_execution_mode: 1
---

## 适用场景
- 重命名类/函数/变量/文件
- 提取公共代码到新模块
- 移动文件到新目录
- 接口变更

## 工作流步骤
1. **重构前：建立安全网**
   - 确认当前所有测试通过：
     // turbo
     - `./scripts/core/test -c clang -t Debug`
   - 记录测试基线（通过数/失败数）
   - 建议新建分支：`git checkout -b refactor/<description>`

2. **分析影响范围**
   - 搜索所有引用点：
     - `grep -rn "<old_name>" include/ src/ tests/`
   - 确认哪些文件需要修改
   - 区分公共 API（`include/fqtools/`）和内部实现（`src/`）的影响

3. **执行重构（按类型）**

   **重命名类/函数/变量**：
   - 修改声明和定义
   - 更新所有引用点
   - 更新测试中的引用
   - 更新 CMakeLists.txt（如果文件名变更）

   **提取模块**：
   - 创建新头文件 + 源文件（可用 `/new-module`）
   - 移动代码到新文件
   - 更新 `#include` 路径
   - 更新 CMakeLists.txt

   **移动文件**：
   - 移动文件到新位置
   - 更新所有 `#include` 路径
   - 更新 CMakeLists.txt
   - 如果是公共头文件，更新 `fq.h` 的 include

4. **编译验证**
   // turbo
   - `./scripts/core/build -c clang -t Debug`
   - 修复编译错误直到成功

5. **测试验证**
   - `./scripts/core/test -c clang -t Debug`
   - 对比测试基线：通过数不应减少
   - 如有新增失败，分析是重构 bug 还是测试本身需要更新

6. **代码质量验证**
   - `./scripts/core/lint format`（自动格式化）
   - `./scripts/core/lint format-check`
   - `./scripts/core/lint tidy -b build-clang-debug`（可选）

7. **Release 构建验证**
   - `./scripts/core/build -c clang -t Release`
   - `./scripts/core/test -c clang -t Release`

8. **收尾**
   - 如该重构需要纳入项目维护历史，按根目录 `CHANGELOG.md` 的维护流程补充摘要
   - 确认 `git diff --stat` 范围合理
   - 提交：`git commit -m "refactor(<scope>): <description>"`

## 安全原则
- **小步迭代**：每步只做一件事，编译通过后再做下一步
- **测试驱动**：每步都要跑测试
- **接口谨慎**：`include/fqtools/` 下的变更影响所有下游
- **可回滚**：保持 git 历史清晰，必要时可 `git revert`
