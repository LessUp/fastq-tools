---
title: "/run-cli-e2e"
description: "运行 FastQTools CLI 端到端测试（tests/e2e/test_cli.sh），用于验证命令行基本回归。"
auto_execution_mode: 1
---

## 适用场景
- 你改动了 `src/cli/` 或命令行参数解析
- 你需要快速验证 `filter/stat` 子命令是否还能跑起来

## 工作流步骤
1. **构建可执行文件**
   - 若未构建：`./scripts/build.sh clang Release`
   - 默认可执行文件路径通常为：`build-clang-release/FastQTools`

2. **运行 E2E**
   - 直接运行：`./tests/e2e/test_cli.sh`

3. **可选：指定可执行文件**
   - 若你的可执行文件不在默认位置：
     - `FASTQTOOLS=/abs/path/to/FastQTools ./tests/e2e/test_cli.sh`

4. **注意事项**
   - 该脚本对部分子命令存在“允许失败”的基线逻辑（会以 WARN 提示），用于回归跟踪
