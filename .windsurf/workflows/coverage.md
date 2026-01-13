---
title: "/coverage"
description: "构建并运行 FastQTools 覆盖率（lcov + HTML）。"
auto_execution_mode: 1
---

## 适用场景
- 你改了核心逻辑，希望看覆盖率变化
- 你需要生成可浏览的 HTML 覆盖率报告

## 工作流步骤
1. **安全前置**
   // turbo
   - `git status -sb`

2. **构建 Coverage（建议 GCC）**
   - 构建：
     - `./scripts/core/build -c gcc -C`

3. **运行测试并生成报告**
   - `./scripts/core/test -c gcc -C --coverage-output coverage`

4. **查看报告**
- HTML：`coverage/html/index.html`

## 注意事项
- 依赖：`lcov`/`genhtml`（通常 `sudo apt-get install lcov`）
- 覆盖率构建会生成大量 `*.gcda/*.gcno`，属于正常现象
