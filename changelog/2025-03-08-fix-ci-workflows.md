# 修复 CI 工作流配置

**日期**: 2025-03-08
**类型**: fix(ci)
**范围**: GitHub Actions 工作流

## 变更内容

### ci.yml
- 修复脚本路径：`scripts/build.sh` → `scripts/core/build`
- 修复脚本路径：`scripts/test.sh` → `scripts/core/test`
- 修复脚本路径：`scripts/lint.sh` → `scripts/core/lint`
- 修复参数格式以匹配新脚本接口（`-c`/`-t` 标志）
- 修复 `${{ matrix.build_type,, }}` 无效的 Actions 表达式语法

### quality.yml
- 修复分支配置：添加 `master` 到触发分支列表（项目实际使用 `master` 分支）

### release.yml
- 新增 `ci-gate` job：使用 `lewagon/wait-on-check-action` 等待 CI 工作流全部通过
- `build` job 现在依赖 `ci-gate`，确保质量门禁通过后才允许构建和发布

## 问题根因

Release 工作流在无任何 CI 前置检查的情况下就能发布，且 quality.yml 的分支配置
未包含项目实际使用的 `master` 分支，导致质量门禁从未被触发。
