# 2026-04-17-fix-github-workflows

## 目标
修复 GitHub Actions 工作流中的全部报错问题

## 主要改动

### 1. `.github/workflows/quality.yml`
- **格式检查修复**：移除了 format-check job 中不正确的 `ref: ${{ github.head_ref || github.ref_name }}` checkout 参数，该参数在 PR 场景下会导致 checkout 失败
- **ASan 测试修复**：在 ASan 测试运行步骤中添加 `--exclude-regex "cmake_package_consumer"`，跳过 cmake_package_consumer 测试（因为 consumer 可执行文件没有 ASan 标志，导致链接失败）
- **覆盖率测试修复**：同样在覆盖率测试运行步骤中添加 `--exclude-regex "cmake_package_consumer"`，原因同上

### 2. `requirements-docs.txt`
- 添加了缺失的 `mkdocs-plotly-plugin==0.1.3` 依赖，修复 Deploy Documentation 工作流中 "No module named 'mkdocs_plotly_plugin'" 错误

## 影响范围
- CI/CD：Code Quality、Deploy Documentation 工作流
- 无代码变更，仅修复工作流配置和文档依赖

## 验证方式
- 推送此分支到远程，观察 CI、Code Quality、Deploy Documentation 工作流是否通过
- 或使用 `gh run watch` 监控工作流状态
