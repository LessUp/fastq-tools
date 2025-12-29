# 2025-12-29 文档与示例一致性修复

## 变更摘要

- 修复并统一了文档/示例与仓库现实不一致的描述与命令参数。
- 增加审计记录，先列出不一致项，再逐步修复。

## 变更内容

- `docs/audit/2025-12-29-docs-style/issues.md`
  - 新增：记录修复前发现的不一致项（命名口径、示例脚本与 CLI 参数/可执行名不一致、引用不存在脚本等）。

- `docs/dev/coding_standards.md`
  - 调整：命名约定更贴合仓库现状（局部变量/参数允许 `snake_case`）。
  - 新增：明确行业惯例文件名与构建生成物目录为“命名例外”。

- `docs/dev/build.md`
  - 调整：移除鼓励性 `rm -rf build-*` 清理命令，改为推荐使用 `scripts/build.sh` 统一管理构建目录。

- `examples/basic_usage/README.md`
  - 调整：移除不存在的示例脚本与下载链接，改为使用仓库内 `tools/data` 样例数据与 `scripts/build.sh` 的实际构建流程。

- `examples/basic_usage/basic_stats.sh`
  - 修复：默认调用实际可执行文件 `FastQTools`（支持 `FASTQTOOLS` 环境变量覆盖，并尝试默认构建产物路径）。
  - 修复：对齐 CLI 参数（使用 `--threads` / `--batch-size`）。
  - 调整：启用严格模式 `set -euo pipefail`，避免未定义变量导致脚本失败。
