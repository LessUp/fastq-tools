---
title: "/quick-build"
description: "快速增量构建 + 运行上次失败的测试，适合日常迭代开发。"
auto_execution_mode: 1
---

## 适用场景
- 修改了少量代码，想快速验证编译+测试
- 上次测试有失败项，修复后想快速复跑

## 工作流步骤
1. **增量构建（不 clean）**
   // turbo
   - `./scripts/core/build -c clang -t Debug`
   - 如果构建失败，检查编译错误并修复后重试

2. **运行测试**
   - 如果有明确的失败用例，用 filter 缩小范围：
     - `./scripts/core/test -c clang -t Debug -f "<pattern>"`
   - 否则运行全部：
     - `./scripts/core/test -c clang -t Debug`

3. **如果测试失败**
   - 从输出中提取失败用例名
   - 定位对应源文件，分析失败原因
   - 修复后回到第 1 步（增量构建 + 重跑失败用例）

4. **通过后（可选）快速 lint**
   // turbo
   - `./scripts/core/lint format-check`

## 备注
- 此工作流**不会** clean 构建目录，利用增量编译加速
- 如果遇到奇怪的链接错误，尝试 `./scripts/core/build -c clang -t Debug --clean`
- Release 验证请用 `/run-tests-and-fix` 或 `/commit-and-push`
