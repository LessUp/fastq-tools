# 2025-12-24 新增与优化 Windsurf Workflows

> 本文件合并自 3 个同日同主题的独立 changelog 条目。

## 背景

为 FastQTools 增加贴合仓库现有脚本的 Windsurf Workflow 工作流入口，并对代码风格检查流程进行升级。

## 变更

### 1. 新增 Workflows

| Workflow | 文件 | 用途 |
|----------|------|------|
| `/code-formatting` | `code-formatting.md` | 运行 `scripts/lint.sh`（format/lint） |
| `/run-tests-and-fix` | `run-tests-and-fix.md` | 运行测试并按失败逐项修复 |
| `/run-cli-e2e` | `run-cli-e2e.md` | 运行 E2E 测试验证 CLI 回归 |
| `/address-pr-comments` | `address-pr-comments.md` | 拉取 PR 评论逐条处理 |
| `/release-package` | `release-package.md` | 生成发布包到 `dist/` |
| `/code-style-review` | `code-style-review.md` | 端到端代码风格检查流程 |

### 2. 代码风格流程升级

- `/code-style-review` 升级为可执行的端到端流程（格式化→复检→静态分析→测试→记录）
- `scripts/lint.sh` 源码扫描范围覆盖 `src/`、`include/`、`tests/`，移除不存在的 `app/`
- `CMakeLists.txt` 将 `BUILD_BENCHMARKS` 默认改为 OFF

### 3. clang-format 环境适配

- `scripts/lint.sh` 支持 `CLANG_FORMAT` 环境变量或自动探测 `clang-format-19`
- `docs/dev/coding_standards.md` 记录具体风格与手动格式化命令

## 影响

- 仅新增/修改 workflow 文档和脚本配置，不改变核心源码逻辑
