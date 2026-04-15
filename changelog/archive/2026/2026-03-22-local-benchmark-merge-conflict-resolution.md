# 2026-03-22 本地 benchmark 改动合并冲突修复

## 背景
本地 `master` 分支落后远程 `origin/master` 81 个提交，同时存在一组未提交的 benchmark 相关改动，导致 `git pull` 被拒绝并进入本地改动与远程重构后的路径/文档冲突状态。

## 处理过程
- 先将本地未提交改动临时保存到 stash：`claude-fastq-tools-merge-fix-2026-03-22`
- 执行远程快进同步
- 回放本地改动并手动解决冲突

## 已解决的冲突文件
- `.dockerignore`
- `.github/workflows/benchmark.yml`
- `.gitignore`
- `.windsurf/rules.md`
- `README.md`
- `docs/performance/benchmark-report.md`
- `scripts/README.md`
- `scripts/tools/benchmark`

## 合并原则
- 以远程最新仓库结构为准：保留 `docs/benchmark-reports/`、`tools/benchmark/scripts/` 等新布局
- 尽量保留本地 benchmark 相关增强点，如编译器对比/可视化入口与相关文档说明
- 清理所有冲突标记，恢复到正常工作区状态

## 当前状态
- 仓库已不处于 merge 过程中
- 工作区仍有未提交修改，这是已整合后的正常状态
- 原始 stash 仍保留，便于人工兜底回溯；在确认无误前不要再次 `stash pop`
