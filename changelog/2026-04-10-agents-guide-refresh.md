# 2026-04-10 AGENTS 指南精简与校准

## 目标
- 将根目录 `AGENTS.md` 从宽泛的项目总览改为更适合自动化编码 Agent 使用的操作指南。
- 明确构建、lint、测试与单测运行方式，并补充当前仓库规则文件状态。

## 变更内容
- 重写 `AGENTS.md`，聚焦以下信息：
  - `scripts/core/build`、`scripts/core/test`、`scripts/core/lint` 的首选命令。
  - 单个 CTest 目标与单个 GTest case 的精确运行方式。
  - `.clang-format`、`.clang-tidy`、`.editorconfig` 和现有源码共同体现的风格约束。
  - 公共 API、错误处理、日志、测试放置位置与 changelog 要求。
- 明确说明当前仓库未发现 Cursor 规则或 Copilot 指令文件。

## 影响
- 不影响构建、测试或运行时行为。
- 后续 Agent 在仓库内执行任务时，可以更快选用正确命令并遵守当前代码风格。

## 验证
- 对照检查了以下实际文件与脚本：
  - `scripts/core/build`
  - `scripts/core/test`
  - `scripts/core/lint`
  - `CMakePresets.json`
  - `.clang-format`
  - `.clang-tidy`
  - `.editorconfig`
  - `tests/CMakeLists.txt`
