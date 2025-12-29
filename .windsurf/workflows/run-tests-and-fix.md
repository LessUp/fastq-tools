---
title: "/run-tests-and-fix"
description: "在 FastQTools 上运行构建+单测/覆盖率，并按失败信息逐项定位与修复。"
auto_execution_mode: 1
---

## 适用场景
- 你修改了核心逻辑，想要在提交前跑完测试
- CI 报错，你需要在本地复现并修复

## 工作流步骤
1. **安全前置**
   // turbo
   - `git status -sb`
   - 若你不希望污染当前分支，建议先新建分支：`git checkout -b fix/tests-YYYYMMDD`

2. **选择测试模式（推荐从轻到重）**
   - **快速单测（默认）**：`./scripts/test.sh -c clang -t Debug`
   - **Release 单测**：`./scripts/test.sh -c clang -t Release`
   - **仅跑匹配用例**：`./scripts/test.sh -c clang -t Debug -f "<pattern>"`
   - **覆盖率（较慢，且需要 lcov/genhtml）**：`./scripts/test.sh -c clang -t Coverage -C`

3. **如果构建目录不存在，脚本会自动先构建**
   - `scripts/test.sh` 会在缺少构建目录时调用 `./scripts/build.sh`
   - 注意：`scripts/build.sh` 会清理并重建对应 `build-*` 目录（不会改源码）

4. **逐个失败项处理（一次只处理一个）**
   - 从失败输出中提取：测试名、断言信息、堆栈（如果有）
   - 定位对应测试文件/模块
   - 若无法判断预期行为：先询问你要以“修代码”还是“改测试”为准
   - 修复后重复第 2 步（建议先用 filter 模式缩小范围）

5. **收尾**
   - 确认 `./scripts/lint.sh format-check` 通过
   - 确认 `./scripts/lint.sh lint --build-dir build-clang-release` 通过（如需要）
