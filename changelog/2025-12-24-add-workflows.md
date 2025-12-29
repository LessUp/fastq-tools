# 新增项目 Workflows（2025-12-24）

## 背景
- 依据请求：参考示例 Workflows，为 FastQTools 增加“贴合仓库现有脚本、能直接执行”的工作流入口。

## 本次变更
- 新增以下 Windsurf Workflows（均复用仓库已有脚本，避免发明新命令，保持 KISS）：
  - `/.windsurf/workflows/code-formatting.md`
    - `/code-formatting`：统一入口运行 `scripts/lint.sh`（format-check / format / lint / lint-fix）。
  - `/.windsurf/workflows/run-tests-and-fix.md`
    - `/run-tests-and-fix`：运行 `scripts/test.sh`（可选 Debug/Release/Coverage/过滤测试）并按失败逐项修复。
  - `/.windsurf/workflows/run-cli-e2e.md`
    - `/run-cli-e2e`：运行 `tests/e2e/test_cli.sh` 验证 CLI 基本回归。
  - `/.windsurf/workflows/address-pr-comments.md`
    - `/address-pr-comments`：使用 `gh` + `jq` 拉取 PR 行级评论，按“逐条处理”方式迭代修复。
  - `/.windsurf/workflows/release-package.md`
    - `/release-package`：运行 `scripts/package_release.sh` 生成发布包，产物输出到 `dist/`。

## 影响范围
- 仅新增 workflow 文档文件，不修改源码逻辑。
- Workflows 复用现有脚本：
  - `scripts/lint.sh`
  - `scripts/build.sh`
  - `scripts/test.sh`
  - `tests/e2e/test_cli.sh`
  - `scripts/package_release.sh`

## 使用方式
- 在仓库根目录通过 slash command 调用：
  - `/code-formatting`
  - `/run-tests-and-fix`
  - `/run-cli-e2e`
  - `/address-pr-comments`
  - `/release-package`
