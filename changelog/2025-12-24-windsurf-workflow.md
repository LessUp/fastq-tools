# 新增 Windsurf Workflow（2025-12-24）

## 目的
- 依据项目要求，为 Cascade 提供可复用的编码风格检查操作指引。
- 结合 `docs/dev/coding_standards.md`、`.clang-format`、`.clang-tidy` 与脚本工具形成系统性流程。

## 主要改动
- 新建 `.windsurf/workflows/code-style-review.md`：
  - 采用官方模板（YAML frontmatter + Markdown步骤）。
  - 定义初始化、规范复核、文件检查、脚本运行、问题修复、总结等 6 个阶段。
  - 明确命令与注意事项（以 `./scripts/lint.sh`、`./scripts/build.sh` 为主），并补充范围控制与安全前置检查。

## 影响
- 团队成员可通过 `/code-style-review` 快速调用统一流程，降低遗漏风险。
- 为后续自动化/CI 流程扩展提供基础。

## 回退方案
- 若不再需要该 workflow，可删除 `.windsurf/workflows/code-style-review.md`。

## 补充记录（2025-12-24）
- 优化 `/code-style-review` 工作流：明确根目录执行、优先自动化检查、提供命名与结构巡检命令、强调修复与记录步骤。
- 尝试执行 `./scripts/lint.sh format-check` 时遇到文件未设可执行权限及缺少 `clang-format`；需先为脚本加执行权限并安装 `clang-format` 后再跑格式检查。
- 调整 `scripts/lint.sh` 以支持通过 `CLANG_FORMAT` 环境变量或自动探测 `clang-format-19`；随后使用 `CLANG_FORMAT=/usr/bin/clang-format-19 ./scripts/lint.sh format` 全量格式化并通过 `format-check`。
- 在 `docs/dev/coding_standards.md` 记录 clang-format 具体风格及手动统一命令。
- 修正 `scripts/lint.sh` 的源码扫描范围：纳入 `include/`、`src/`、`tests/`，确保全局格式化覆盖公共头文件。
